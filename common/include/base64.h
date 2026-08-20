#ifndef BASE_64_H__
#define BASE_64_H__

#include <string>

namespace comm
{
	class Base64
	{
	public:
		static void enBASE64code( char *charBuf,    int charBufLen,    char *base64Char,int &base64CharLen);
		static void deBASE64code( char *base64Char,  int base64CharLen, char *outStr,   int &outStrLen);

		static std::string decode(const std::string& indata);
		static std::string encode(const std::string& indata);
	};	
}
#endif
