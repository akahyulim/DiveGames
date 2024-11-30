#include "stdafx.h"
#include "StringUtils.h"
#include "FileUtils.h"

namespace Dive
{
	std::string StringUtils::ToUpperCase(const std::string& str)
	{
		auto copy = str;
		for (int32_t i = 0; i < (int32_t)copy.size(); ++i)
		{
			if ('a' <= copy[i] && 'z' >= copy[i])
			{
				copy[i] -= 32;
			}
		}

		return copy;
	}

	std::string StringUtils::ToLowerCase(const std::string& str)
	{
		auto copy = str;
		for (int32_t i = 0; i < (int32_t)copy.size(); ++i)
		{
			if ('A' <= copy[i] && 'Z' >= copy[i])
				copy[i] += 32;
		}

		return copy;
	}

	std::string StringUtils::StringLeftTrim(const std::string& str, const char* pTarget)
	{
		auto copy = str;
		copy.erase(0, copy.find_first_not_of(pTarget));

		return copy;
	}

	std::string StringUtils::StringRightTrim(const std::string& str, const char* pTarget)
	{
		auto copy = str;
		copy.erase(copy.find_last_not_of(pTarget) + 1);

		return copy;
	}

	std::string StringUtils::StringTrim(const std::string& str, const char* pTarget)
	{
		return StringLeftTrim(StringRightTrim(str, pTarget), pTarget);
	}

	std::string StringUtils::AddTrailingSlash(const std::string& pathName)
	{
		if (pathName.empty())
			return std::string();

		// \\를 모두 /로 변경
		auto ret = FileUtils::GetInternalPath(pathName);

		if (ret.back() != '/')
			ret += '/';

		return ret;
	}

	std::string StringUtils::RemoveTrailingSlash(const std::string& pathName)
	{
		if (pathName.empty())
			return std::string();

		auto ret = pathName;

		if (ret.back() == '/')
			ret.resize(ret.length() - 1);

		return ret;
	}

	std::string StringUtils::StringReplace(const std::string& str, const std::string& target, const std::string& replace)
	{
		if (str.empty())
			return std::string();

		auto ret = str;
		size_t pos = 0;
		while ((pos = ret.find(target)) != std::string::npos)
		{
			ret.replace(pos, target.size(), replace);
		}

		return ret;
	}

	std::wstring StringUtils::StringToWString(const std::string& str)
	{
		if (str.empty())
			return std::wstring();

		auto size = ::MultiByteToWideChar(
			CP_ACP,
			0,
			str.c_str(),
			static_cast<int32_t>(str.size()),
			nullptr,
			0);

		std::wstring ret;
		ret.resize(size);

		::MultiByteToWideChar(
			CP_ACP,
			0,
			str.c_str(),
			static_cast<int32_t>(str.size()),
			const_cast<wchar_t*>(ret.c_str()),
			static_cast<int32_t>(ret.size()));

		return ret;
	}

	std::string StringUtils::WStringToString(const std::wstring& str)
	{
		if (str.empty())
			return std::string();

		auto size = ::WideCharToMultiByte(
			CP_ACP,
			0,
			str.c_str(),
			static_cast<int32_t>(str.size()),
			nullptr,
			0,
			nullptr,
			nullptr);

		std::string ret;
		ret.resize(size);

		::WideCharToMultiByte(
			CP_ACP,
			0,
			str.c_str(),
			static_cast<int32_t>(str.size()),
			const_cast<char*>(ret.c_str()),
			static_cast<int32_t>(ret.size()),
			nullptr,
			nullptr);

		return ret;
	}

	std::vector<std::string> StringUtils::StringSplit(const std::string& str, char seperator)
	{
		std::vector<std::string> ret;

		size_t previous = 0;
		size_t current = str.find(seperator);
		if (current == std::string::npos)
			ret.emplace_back(str);
		else
		{
			while (current != std::string::npos)
			{
				ret.emplace_back(str.substr(previous, current - previous));
				previous = current + 1;
				current = str.find(seperator, previous);
			}
			ret.emplace_back(str.substr(previous, current - previous));
		}

		return ret;
	}
}