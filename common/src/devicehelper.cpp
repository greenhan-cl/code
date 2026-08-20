#include "devicehelper.h"
#include "strutil.h"
#include "processhelper.h"
#include "fileutil.h"
#include "iputil.h"
#include "mysock.h"
#include <chrono>
#include <thread>

using namespace std;
using namespace comm;

DeviceHelper::DeviceHelper(comm::Logger* logger)
{
	m_logger = logger;
}

string DeviceHelper::getDnsServer()
{
#if defined(_WIN32)
	return "";
#else
	ProcessHelper procHelper(m_logger);

	StringArray tmpServerList;
	StringArray serverList;


	if (!FileUtil::file_exist(DNS_FILENAME)) {
		return "";
	}

	string cmd = string("grep -e \"^") + string(NAMESERVER_TAG) +  "\" " + string(DNS_FILENAME) + " | awk \'{print $2}\' | " + HEAD_CMD + " -1";
	string dns_server = procHelper.runShellTrim(cmd.c_str());
	return dns_server;
#endif
}

std::string DeviceHelper::getDnsServerListPrefix(bool ipv6Prefix, bool onlyOne, bool useCache)
{
#if defined(_WIN32)
	return "";
#else

	ProcessHelper procHelper(m_logger);
	char cmd[1024];
	static string dns_server_onlyone_ipv4 = "";
	static string dns_server_onlyone_ipv6 = "";
	static string dns_server_all_ipv4 = "";
	static string dns_server_all_ipv6 = "";

	if (useCache)
	{
		string servers = "";
		if (ipv6Prefix)
		{
			servers = onlyOne?dns_server_onlyone_ipv6:dns_server_all_ipv6;
		}
		else
		{
			servers = onlyOne?dns_server_onlyone_ipv4:dns_server_all_ipv4;
		}

		if (servers != "")
		{
			return servers;
		}
	}

	StringArray dns_server_list;
	if (ipv6Prefix)
	{
		StringArray dns_server_list1;
		snprintf(cmd, sizeof(cmd), "grep -e \"^%s\" %s | awk \'{print $2}\' | grep ':' ", NAMESERVER_TAG, DNS_FILENAME);
		if(procHelper.runShellTrim(cmd, dns_server_list1)){
			dns_server_list.insert(dns_server_list.end(), dns_server_list1.begin(), dns_server_list1.end());
		}

		StringArray dns_server_list2;
		snprintf(cmd, sizeof(cmd), "grep -e \"^%s\" %s | awk \'{print $2}\' | grep -v ':' ", NAMESERVER_TAG, DNS_FILENAME);
		if(procHelper.runShellTrim(cmd, dns_server_list2)){
			dns_server_list.insert(dns_server_list.end(), dns_server_list2.begin(), dns_server_list2.end());
		}
	}
	else
	{
		StringArray dns_server_list1;
		snprintf(cmd, sizeof(cmd), "grep -e \"^%s\" %s | awk \'{print $2}\' | grep -v ':' ", NAMESERVER_TAG, DNS_FILENAME);
		
		if(procHelper.runShellTrim(cmd, dns_server_list1)){
			dns_server_list.insert(dns_server_list.end(), dns_server_list1.begin(), dns_server_list1.end());
		}

		StringArray dns_server_list2;
		snprintf(cmd, sizeof(cmd), "grep -e \"^%s\" %s | awk \'{print $2}\' | grep ':' ", NAMESERVER_TAG, DNS_FILENAME);
		if(procHelper.runShellTrim(cmd, dns_server_list2)){
			dns_server_list.insert(dns_server_list.end(), dns_server_list2.begin(), dns_server_list2.end());
		}
	}

	if (!dns_server_list.empty())
	{
		if (ipv6Prefix)
		{
			dns_server_onlyone_ipv6 = dns_server_list[0];
			dns_server_all_ipv6 = StrUtil::combine(dns_server_list, ',');
			return onlyOne ? dns_server_onlyone_ipv6 : dns_server_all_ipv6;
		}
		else
		{
			dns_server_onlyone_ipv4 = dns_server_list[0];
			dns_server_all_ipv4 = StrUtil::combine(dns_server_list, ',');
			return onlyOne ? dns_server_onlyone_ipv4 : dns_server_all_ipv4;
		}
	}	
	else
	{
		return "";
	}
#endif
}

string DeviceHelper::getMac()
{
	static string mac = "";
	if (mac.empty())
	{
		mac = getMac_real();
	}
	return mac;
}

#if defined(_WIN32)
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

bool getfirstmac_win32(std::string& first_mac)
{
	bool ret = false;

	ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
	PIP_ADAPTER_ADDRESSES pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
	if (pAddresses == NULL) 
		return false;

	// Make an initial call to GetAdaptersAddresses to get the necessary size into the ulOutBufLen variable
	if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
		if (pAddresses == NULL) 
			return false;
	}

	if(GetAdaptersAddresses(AF_UNSPEC, 0, NULL, pAddresses, &outBufLen) == NO_ERROR)
	{
		// If successful, output some information from the data we received
		for(PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != NULL; pCurrAddresses = pCurrAddresses->Next)
		{
			if(pCurrAddresses->PhysicalAddressLength != 6)
				continue;
			char acMAC[32];
			sprintf(acMAC, "%02X:%02X:%02X:%02X:%02X:%02X",
				int (pCurrAddresses->PhysicalAddress[0]),
				int (pCurrAddresses->PhysicalAddress[1]),
				int (pCurrAddresses->PhysicalAddress[2]),
				int (pCurrAddresses->PhysicalAddress[3]),
				int (pCurrAddresses->PhysicalAddress[4]),
				int (pCurrAddresses->PhysicalAddress[5]));
			first_mac = acMAC;
			ret = true;
			break;
		}
	} 

	free(pAddresses);
	return ret;
}
#endif

string DeviceHelper::getMac_real()
{
#if defined(__LINUX__) && !defined(ANDROID)
	if (!FileUtil::file_exist("/sbin/ifconfig")) {
		return "";
	}

	ProcessHelper processHelper(m_logger);
	char mac[100] = {'\0'};
	int len = processHelper.runShell("/sbin/ifconfig|grep -e HWaddr -e ether|head -1|grep -oE '[a-f0-9A-F]{2}(:[a-f0-9A-F]{2}){5}'", mac, sizeof(mac));
	if (len >= 1) {
		mac[len-1] = '\0';
	}
	string ret = mac;
	StrUtil::toUpper(ret);

	if (ret.empty()) {
		ret = "XX-XX-XX-XX-XX-XX";
	}

	return ret;
#elif defined(_WIN32)
	string mac;
	bool ret = getfirstmac_win32(mac);
	return ret ? mac : "XX-XX-XX-XX-XX-XX";
#endif
    return "XX-XX-XX-XX-XX-XX";
}

string DeviceHelper::getpppoeident()
{
	return "";
}

string DeviceHelper::getSN()
{
	return "";
}

string DeviceHelper::getLOID()
{
	return "";
}

string DeviceHelper::getUpMac()
{
	return "";
}

std::string DeviceHelper::getModel(){
	return PLAT_MODEL;
}

std::string DeviceHelper::getSubModel()
{

	return PLAT_EXTRA;

}

int DeviceHelper::getWANSpeed()
{
	return 0;
}

std::string DeviceHelper::getWanEth()
{
#if defined(_WIN32)
	return "";
#else
	static string wan_eth = "";
	if (wan_eth == "")
	{
		ProcessHelper processHelper(m_logger);
		string iproute_line = processHelper.runShellTrim(string("ip route | grep default | ") + HEAD_CMD + " -1");
		wan_eth = StrUtil::getSubBetween(iproute_line, "dev ", " ");
		StrUtil::trim(wan_eth);

		if (wan_eth == "")
		{
			wan_eth = StrUtil::getSubBetween(iproute_line, "dev ", "");
			StrUtil::trim(wan_eth);
		}

		if (m_logger != NULL)
		{
			m_logger->logFmt("getWanEth:%s", wan_eth.c_str());
		}
	}	
	return wan_eth;
#endif
}

void DeviceHelper::getWanEthMany(std::vector<std::string>& eths)
{
#if defined(CHECK_MANY_WAN)
	static string wan_eth = "";
	if (wan_eth == "")
	{
		ProcessHelper processHelper(m_logger);
		wan_eth= processHelper.runShellTrim("ip route | grep default | awk '{print $5}' ");

		StrUtil::replace(wan_eth, "\n", ",");
		if (m_logger != NULL)
		{
			m_logger->logFmt("wan_eth:%s", wan_eth.c_str());
		}
	}

	StrUtil::spilt(wan_eth, ",", eths);	
	if (eths.size() > MAX_WAN_COUNT)
	{
		eths.erase(eths.begin() + MAX_WAN_COUNT, eths.end());
	}
#else
	string wan_eth = getWanEth();
	eths.push_back(wan_eth);
#endif
}

std::string DeviceHelper::getLocalIp()
{
	char* local_ip = NULL;
	string wan_eth = getWanEth();
	if (wan_eth != "")
	{
		local_ip = mysock_if2ip(wan_eth.c_str());
#if defined(__LINUX__) && !defined(ANDROID)
		if (strcmp(local_ip, "127.0.0.1") == 0)
		{
			string addr_line = ProcessHelper(m_logger).runShellTrim("hostname -I");
			if (m_logger != NULL)
			{
				m_logger->logFmt("addr_line:%s", addr_line.c_str());
			}

			StringArray addrs;
			StrUtil::spilt2(addr_line, ' ', addrs);
			for (int i=0; i<(int)addrs.size(); i++)
			{
				if (IpUtil::isValidIp(addrs[i].c_str()) && addrs[i] != "127.0.0.1")
				{
					return addrs[i];
				}
			}
		}
#endif
	}

	if (local_ip != NULL)
	{
		return local_ip;
	}
	else
	{
		return "";
	}
}

std::string DeviceHelper::getLocalIpv6()
{
	string wan_eth = getWanEth();
	if (wan_eth != "")
	{
		return mysock_if2ipv6(wan_eth.c_str());
	}
	return "";
}

std::string DeviceHelper::getLanIp()
{
	return "";
}

int DeviceHelper::getMemFree()
{
	ProcessHelper procHelper(NULL);

#if defined(__LINUX__)
	string mem_free_line = procHelper.runShell("cat /proc/meminfo | grep 'MemFree:'");
	string mem_free_str = StrUtil::getSubBetween(mem_free_line, "MemFree:", "kB");
	StrUtil::trim(mem_free_str);
	if (mem_free_str.empty())
	{
		return -1;
	}
	return StrUtil::Str2Int(mem_free_str);
#else
	return -1; //not implete
#endif
}

int DeviceHelper::getExeMem(const char* proc)
{
	ProcessHelper procHelper(NULL);

#if defined(__LINUX__)
	string cmd = "cat /proc/$pid/status | grep VmSize: | awk \'{print $2}\'";
	StrUtil::replace(cmd, "$pid", StrUtil::Int2Str(getpid()).c_str());

	string memDataStr = procHelper.runShell(cmd.c_str());
	if (memDataStr.empty())
	{
		return -1;
	}

	StrUtil::trim(memDataStr);
	return StrUtil::Str2Int(memDataStr);
#else
	return -1; //not implete
#endif
}

bool DeviceHelper::getCpuSlice(INT64& used, INT64& all)
{
	ProcessHelper procHelper(NULL);

	string cmd;

#if defined(__LINUX__)
	cmd = string("cat /proc/stat | grep cpu | ") + HEAD_CMD +  " -1";
#else
	return false;
#endif

	INT64 all1= 0, idle1= 0, used1=0;

	string memDataStr = procHelper.runShell(cmd.c_str());
	if (memDataStr.empty())
	{
		return false;
	}

	std::vector<std::string> statVec;
	StrUtil::spilt2(memDataStr, ' ', statVec);

	for(unsigned int i = 1;i <statVec.size() && i<8;i++){
		all1 = all1 + StrUtil::Str2Int64(statVec[i]);
	}
	idle1 = StrUtil::Str2Int64(statVec[4]);
	used1 = all1 - idle1;

	all = all1;
	used = used1;
	return true;
}

//cpu 40126 0 78782 236028937 264 0 50621 0 0 0
//user + nice + system + idle + iowait + irq + softirq
float DeviceHelper::getUsedCpu()
{
	INT64 all1=0, all2=0, used1=0, used2=0;

	if(!getCpuSlice(used1, all1)){
		return -1;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
	if(!getCpuSlice(used2, all2)){
		return -1;
	}

	INT64 used = used2 - used1;
	if(used > 0){
		return (float)(used) / (all2-all1) * 100;
	}

	return 0;
}

void DeviceHelper::getWanFlow(int seconds, int& down_bps, int& up_bps)
{
	StringArray eths;
	getWanEthMany(eths);
	string wan_eth = StrUtil::combine(eths, ',');

	INT64 down_bytes_1 = 0;
	INT64 up_bytes_1 = 0;
	getDevBytes(wan_eth, down_bytes_1, up_bytes_1);

	std::this_thread::sleep_for(std::chrono::milliseconds(seconds * 1000));

	INT64 down_bytes_2 = 0;
	INT64 up_bytes_2 = 0;
	getDevBytes(wan_eth, down_bytes_2, up_bytes_2);

	down_bps = (int)(down_bytes_2 - down_bytes_1) / seconds * 8;
	up_bps =    (int) (up_bytes_2 - up_bytes_1) / seconds * 8;	
}

void DeviceHelper::getDevBytes(const string& dev, INT64& down_bytes, INT64& up_bytes)
{
	StringArray eths;
	StrUtil::spilt(dev, ",", eths);
	string content = FileUtil::file_read("/proc/net/dev");

	INT64 down_bytes_total = 0;
	INT64 up_bytes_total = 0;
	for (int i=0; i<(int)eths.size(); i++)
	{
		string str_down = StrUtil::awk(StrUtil::grep(content, eths[i]), 2);
		down_bytes_total += StrUtil::Str2Int64(str_down);

		string str_up = StrUtil::awk(StrUtil::grep(content, eths[i]), 10);
		up_bytes_total += StrUtil::Str2Int64(str_up);
	}

	down_bytes = down_bytes_total;
	up_bytes = up_bytes_total;
}

string DeviceHelper::getBasePath(){
#if defined(DLSPEED_PATH)
	return DLSPEED_PATH;
#else
	return PathUtil::GetExePathA();
#endif
}

string DeviceHelper::getTempPath()
{
	return getBasePath() + "temp" + PATH_SEPA;
}

string DeviceHelper::getLogPath(){
	return getBasePath() + "log" + PATH_SEPA;
}
