/*
	StringHelper 선언부

	- 아.. 이게 따로 있었구나....

	제작: 서보윤( akahyulim@gmail.com )
*/

#pragma once

//= INCLUDES ====================
#include <string>
#include <Windows.h>
#include <locale>
#include <vector>
//===============================

namespace Dive
{
	namespace StringHelper
	{
		// MBCS
		std::string ToUppercase(const char* pLowerStr);
		std::string ToLowercase(const char* pUpperStr);
		std::string ToUppercase(const std::string& lowerStr);
		std::string ToLowercase(const std::string& upperStr);

		std::string GetStringAfterToken(const char* pStr, const char* pToken);
		std::string GetStringBetweenTokens(const char* pStr, const char* pFirstToken, const char* pSecondToken);
		std::string ReplaceToken(const char* pStr, const char* pToken, const char* pNewToken);

		// UNICODE
		std::wstring MbcsToWbcs(const std::string str, unsigned int mbcsEncoding = CP_ACP);
		std::string WbcsToMbcs(const std::wstring str, unsigned int mbcsEncoding = CP_ACP);
		std::string ConvertMbcs(const std::string str, unsigned int fromEncoding, unsigned int toEncoding);
		std::wstring Utf8ToUtf16(const std::string str);
		std::string Utf16ToUtf8(const std::wstring str);
	}
}