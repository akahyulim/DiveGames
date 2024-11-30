#pragma once

namespace Dive
{
	class StringUtils
	{
	public:
		static std::string ToUpperCase(const std::string& str);
		static std::string ToLowerCase(const std::string& str);
		static std::string StringLeftTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
		static std::string StringRightTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
		static std::string StringTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
		static std::string AddTrailingSlash(const std::string& pathName);
		static std::string RemoveTrailingSlash(const std::string& pathName);
		static std::string StringReplace(const std::string& str, const std::string& target, const std::string& replace);
		static std::wstring StringToWString(const std::string& str);
		static std::string WStringToString(const std::wstring& str);
		static std::vector<std::string> StringSplit(const std::string& str, char seperator);
	};
}