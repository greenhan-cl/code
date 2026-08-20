#include "iputil.h"

using namespace std;
using namespace comm;

/*void IpUtil::adjustIP(std::string& paraStrIP)
{
	std::string strTemp[5] ;
	int i = 0 ;

	strTemp[i] = strtok((char*)paraStrIP.c_str(), ".") ;
	while (!strTemp[i].empty())
	{
		if (1 == strTemp[i].size())
		{
			strTemp[i].insert(0, 2, '0') ;
		}
		else if (2 == strTemp[i].size())
		{
			strTemp[i].insert(0, 1, '0') ;
		}
		if (3 == i)
		{
			break ;
		}
		strTemp[++i] = strtok(NULL, ".")  ;
	}
	paraStrIP.erase() ;
	for (i = 0; i < 4; ++i)
	{
		paraStrIP += strTemp[i] ;
		if (3 != i)
		{
			paraStrIP += "." ;
		}
	}
}*/

void IpUtil::adjustIP(std::string& paraStrIP)
{
	string subStr = paraStrIP + ".";
	string::size_type splitePos = string::npos;

	paraStrIP.erase();
	while((splitePos = subStr.find(".")) != string::npos)
	{
		string segStr = subStr.substr(0, splitePos);
		if (segStr.length() == 1) {
			segStr = "00" + segStr;
		}
		else if (segStr.length() == 2) {
			segStr = "0" + segStr;
		}

		paraStrIP += (segStr + ".");
		subStr = subStr.substr(splitePos + 1);
	}
	StrUtil::removeEndStr(paraStrIP, ".");
}

void IpUtil::fullIpv6Seg(std::string& seg_str)
{
	if (seg_str.empty()) {
		return;
	}

	string subStr = seg_str + ":";
	string::size_type splitePos = string::npos;

	seg_str.erase();
	while((splitePos = subStr.find(":")) != string::npos)
	{
		string segStr = subStr.substr(0, splitePos);
		if (segStr.length() == 1) {
			segStr = "000" + segStr;
		}
		else if (segStr.length() == 2) {
			segStr = "00" + segStr;
		}
		else if (segStr.length() == 3) {
			segStr = "0" + segStr;
		}

		seg_str += (segStr + ":");
		subStr = subStr.substr(splitePos + 1);
	}
	StrUtil::removeEndStr(seg_str, ":");
}

string IpUtil::fullIpv6(const std::string& ip_str)
{
	if (!isValidIpv6(ip_str.c_str()))
	{
		return ip_str;
	}

	string dColonStr = "::";
	string::size_type dColonPos = ip_str.find(dColonStr);
	string first_part = ip_str;
	string second_part = ""; 

	if (dColonPos == string::npos) {
		fullIpv6Seg(first_part);
		return first_part;
	}

	string res_str;
	first_part = ip_str.substr(0, dColonPos);
	second_part = ip_str.substr(dColonPos + dColonStr.length());

	fullIpv6Seg(first_part);
	fullIpv6Seg(second_part);

	string mid_part = "0000";
	int mid_seg_num = (40 - (first_part.empty()?0:(first_part.length()+1)) - (second_part.empty()?0:(second_part.length()+1))) / 5;
	for (int i=1; i<mid_seg_num; i++)
	{
		mid_part += ":0000";
	}

	res_str.erase();
	if (!first_part.empty()) {
		res_str += first_part + ":";
	}

	res_str += mid_part + ":" + second_part;
	StrUtil::removeEndStr(res_str, ":");
	
	return res_str;
}

bool IpUtil::isValidDomain(const char* domain)
{
	/*if (domain==NULL || strcmp(domain, "")==0)
	{
		return false;
	}

	for(int i=0; domain[i] != '\0'; i++)
	{
		if (domain[i] == '.')
		{
			continue; //valid domain, can has.
		}
		else if( (domain[i]<'0') || (domain[i]>'9') ) 
		{				
			return true; //valid domain (if has one is not 0-9)
		}
	}
	return false; //ip format(valid or invalid)*/

	return !isValidIp(domain) && !isValidIpv6(domain);
}

bool IpUtil::isValidIp(const char* ip)
{
	if (ip==NULL || strcmp(ip, "")==0)
	{
		return false;
	}

	int a,b,c,d;
	a = b = c = d = -1;
	sscanf(ip, "%d.%d.%d.%d", &a,&b,&c,&d);

	if(a>=0 && a<256 && b>=0 && b<256 && c>=0 && c<256 && d>=0 && d<256)
	{
		char temp[30];
		sprintf(temp, "%d.%d.%d.%d",a,b,c,d);
		
		if(strcmp(temp, ip)==0)
		{    
			return true;
		}
	}
	return false;
}

bool IpUtil::isValidIpv6(const char* ip)
{
	int colon_count = 0;
	int char_count = 0;
	int colon_repeat_count = 0;
	int colon_double_count = 0;

	char c = '\0';
	while(c = *ip)
	{
		ip ++;
		if ( !(c == ':' || (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) )	//有效字符范围
		{
			return false;
		}

		if (c == ':')
		{
			colon_count++;
			colon_repeat_count++;
			if (colon_repeat_count > 2)		//	0 <= 冒号连续个数 <= 2
			{
				return false;
			}
			else if (colon_repeat_count == 2)
			{
				colon_double_count++;
			}
			char_count = 0;
		}
		else
		{
			if (++char_count > 4)				//	0 <= 冒号间字符个数 <= 4
			{
				return false;
			}
			colon_repeat_count = 0;
		}
	}

	if (colon_count > 7 || colon_count < 2)		//	2 <= 冒号个数 <= 7
	{
		return false;
	}
	
	if (colon_count < 7 && colon_double_count != 1)
	{
		return false;
	}

	if (colon_double_count > 1)					//	"::"个数 <= 1
	{
		return false;
	}

	return true;
}

bool IpUtil::isInnerIp(const std::string& strIP)
{
	if (!isValidIp(strIP.c_str()))
	{
		return false;
	}
	
	std::string str10From("010.000.000.000") ;
	std::string str10To("010.255.255.255") ;

	std::string str172From("172.016.000.000") ;
	std::string str172To("172.031.255.255") ;

	std::string str192From("192.168.000.000") ;
	std::string str192To("192.168.255.255") ;

	//Reserved address
	std::string str100From("100.064.000.000") ;
	std::string str100To("100.127.255.255") ;

	//Linux64 need use c_str(), otherwise will modify strIp, i don't know why
	std::string strIPTemp = strIP.c_str();

	adjustIP(strIPTemp) ;

	if (strIPTemp >= str10From && strIPTemp <= str10To)
	{
		return true ;
	}
	else if (strIPTemp >= str172From && strIPTemp <= str172To)
	{
		return true ;
	}
	else if(strIPTemp >= str192From && strIPTemp <= str192To)
	{
		return true ;
	}
	else if(strIPTemp >= str100From && strIPTemp <= str100To)
	{
		return true ;
	}
	return false ;
}

std::string IpUtil::iptostr(unsigned int ip)
{
	in_addr addr = *((in_addr*)&ip);
	return inet_ntoa(addr);
}

unsigned int IpUtil::strtoip(const std::string& ip_str)
{
	return inet_addr(ip_str.c_str());
}
