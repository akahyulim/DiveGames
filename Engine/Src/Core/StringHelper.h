#pragma once

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

		// snprintf보단 snprintf_s가 안전하다는 경고 메시지가 뜬다.
		template<typename ... Args>
		std::string StringFormat(const std::string& format, Args ... args)
		{
			size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
			std::unique_ptr<char[]> buf(new char[size]);
			snprintf(buf.get(), size, format.c_str(), args...);

			return std::string(buf.get(), buf.get() + size - 1);	
		}

		template<typename ... Args>
		std::wstring WStringFormat(const std::wstring& format, Args ... args)
		{
			size_t size = _snwprintf(nullptr, 0, format.c_str(), args...) + 1;
			std::unique_ptr<wchar_t[]> buf(new wchar_t[size]);
			_snwprintf(buf.get(), size, format.c_str(), args...);

			return std::wstring(buf.get(), buf.get() + size - 1);
		}
	}
}