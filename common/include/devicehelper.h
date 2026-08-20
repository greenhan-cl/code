#ifndef DEVICE_HELPER_H__
#define DEVICE_HELPER_H__

#include "logger.h"
#include <string>
#include <vector>
#include "platinc.h"

namespace comm
{
	class DeviceHelper
	{
	public:
		DeviceHelper(comm::Logger* logger);
		std::string getDnsServer();
		std::string getDnsServerListPrefix(bool ipv6Prefix, bool onlyOne, bool useCache);
		std::string getMac();
		std::string getpppoeident();
		std::string getSN();
		std::string getLOID();
		std::string getUpMac();
		std::string getModel();
		std::string getSubModel();
		int getWANSpeed();
		//wan_ip
		std::string getLocalIp();		
		std::string getLocalIpv6();
		std::string getLanIp();

		std::string getWanEth();
		void getWanEthMany(std::vector<std::string>& eths);

		//return -1 if not implement or failed
		int getMemFree();
		//return -1 if not implement or failed
		int getExeMem(const char* proc);
		float getUsedCpu();
		bool getCpuSlice(INT64& used, INT64& all);
		void getWanFlow(int seconds, int& down_bps, int& up_bps);
		void getDevBytes(const std::string& dev, INT64& down_bytes, INT64& up_bytes);

		static std::string getBasePath();
		static std::string getTempPath();
		static std::string getLogPath();

	private:
		std::string getMac_real();

	private:
		comm::Logger* m_logger;
	};
}

#endif
