#include "base64.h"
#include <string>
#include <cstring>
using namespace std;
using namespace comm;

//refer: https://blog.csdn.net/xuebing1995/article/details/79395881

static const char *ALPHA_BASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; //BASE64  64个字符

void Base64::enBASE64code( char *charBuf,    int charBufLen,    char *base64Char,int &base64CharLen) 
{
	int a = 0;
	int i = 0;
	while (i < charBufLen) 
	{
		char b0 = charBuf[i++];
		char b1 = (i < charBufLen) ? charBuf[i++] : 0;
		char b2 = (i < charBufLen) ? charBuf[i++] : 0;

		int int63 = 0x3F;   //  00111111 意图去掉8bit的两高位
		int int255 = 0xFF;  // 11111111
		base64Char[a++] = ALPHA_BASE[(b0 >> 2) & int63];    //b0字符的前6bit   00+6bit 组成一个十进制数 即BASE编码表号
		base64Char[a++] = ALPHA_BASE[((b0 << 4) | ((b1 & int255) >> 4)) & int63];   //b0 要想取最后两位，所以左移四位 得到最后两位 再和b1字符的 前四个字符做  或运算 组成6bit  00+6bit
		base64Char[a++] = ALPHA_BASE[((b1 << 2) | ((b2 & int255) >> 6)) & int63];   //b1 要想得到后四位  需要右移2位 再和 b2的前4 位做或运算 组成6bit   00+bit 租组成BASE64编码
		base64Char[a++] = ALPHA_BASE[b2 & int63];  //b2的后6位   00+6bit 组成BASE编码号
	}
	base64CharLen = a;	//输出的base64长度
	//最后不够4个字符  补=，最多补两个 ==
	switch (charBufLen % 3) 
	{
	case 1:
		base64Char[--a] = '=';
	case 2:
		base64Char[--a] = '=';
	}
}

void Base64::deBASE64code( char *base64Char,  int base64CharLen, char *outStr,   int &outStrLen) 
{
	int toInt[128] = {-1};
	for (int i = 0; i < 64; i++) //将ANSI 码找到对应的BASE64码
	{
		int pos=ALPHA_BASE[i];
		toInt[pos] = i;
	}
	int int255 = 0xFF;  //11111111
	int index = 0;
	for (int i = 0; i < base64CharLen; i += 4) 
	{
		int c0 = toInt[base64Char[i]]; // 获取一组中第一个BASE64编码号
		int c1 = toInt[base64Char[i + 1]];// 获取一组中第二个BASE64编码号
		outStr[index++] = (((c0 << 2) | (c1 >> 4)) & int255);  //c0后6位  和c1去完前2位的  的前2位  组成8bit 再转换成字符  
		if (index >= base64CharLen)  //如果长度小于输入进来的长度 意图去掉=
		{
			return ;
		}
		int c2 = toInt[base64Char[i + 2]];// 获取一组中第三个BASE64编码号
		outStr[index++] = (((c1 << 4) | (c2 >> 2)) & int255);  //c1的后6位和c2去完前2位的  的前2位 组成8bit  转换成字符
		if (index >= outStrLen)  //如果长度小于输入进来的长度  意图去掉=
		{
			return ;
		}
		int c3 = toInt[base64Char[i + 3]];// 获取一组中第四个BASE64编码号
		outStr[index++] = (((c2 << 6) | c3) & int255); //c2的后2位和c3的后6位  组成8bit 转换成字符
	}
	outStrLen = index;
	if (base64CharLen > 1 && toInt[base64Char[base64CharLen-1]] == 0) outStrLen --;
	if (base64CharLen > 2 && toInt[base64Char[base64CharLen-2]] == 0) outStrLen --;
}

std::string Base64::decode(const std::string& indata)
{
	char outStr[10 * 1024] = {0};
	int outLen = sizeof(outStr);
	deBASE64code((char*)indata.c_str(), indata.length(), outStr, outLen);
	return outStr;	
}

std::string Base64::encode(const std::string& indata)
{
	char outStr[10 * 1024] = {0};
	int outLen = sizeof(outStr);
	enBASE64code((char*)indata.c_str(), indata.length(), outStr, outLen);
	return outStr;	
}
