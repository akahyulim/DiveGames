#pragma once

namespace Dive
{
	class StringUtils
	{
	public:
		static std::string ToUpperCase(const std::string& input);
		static std::string ToLowerCase(const std::string& input);
		static std::string StringLeftTrim(const std::string& input, const char* trimChars = " \t\n\r\f\v");
		static std::string StringRightTrim(const std::string& input, const char* trimChars = " \t\n\r\f\v");
		static std::string StringTrim(const std::string& input, const char* trimChars = " \t\n\r\f\v");
		static std::string RemoveTrailingSlash(const std::string& input);
		static std::string StringReplace(const std::string& input, const std::string& target, const std::string& replacement);
		static std::wstring StringToWString(const std::string& input);
		static std::string WStringToString(const std::wstring& input);
		static std::vector<std::string> StringSplit(const std::string& input, char separator);

	};
}
