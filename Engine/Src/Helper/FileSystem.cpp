#include "divepch.h"
#include "FileSystem.h"

namespace Dive
{
	namespace Helper
	{
		std::string FileSystem::GetDirectory(const std::string& filepath)
		{
			auto index = filepath.find_last_of("\\/");
			if (index == std::string::npos)
				return "";

			return filepath.substr(0, index + 1);
		}

		std::string FileSystem::GetFilepathWithoutExtension(const std::string& filepath)
		{
			auto filename = GetFileName(filepath);
			auto index = filename.find_last_of('.');

			if (index == std::string::npos)
				return "";

			return filename.substr(0, index);
		}

		std::string FileSystem::GetFileName(const std::string& filepath)
		{
			return std::filesystem::path(filepath).filename().generic_string();
		}

		std::string FileSystem::GetFileNameWithoutExtension(const std::string& filepath)
		{
			auto filename = GetFileName(filepath);
			auto index = filename.find_last_of('.');
	
			if (index == std::string::npos)
				return "";

			return filename.substr(0, index);
		}

		// 좀 애매하다.
		std::wstring FileSystem::StringToWString(const std::string& str)
		{
			return std::wstring(str.begin(), str.end());
		}
	}
}