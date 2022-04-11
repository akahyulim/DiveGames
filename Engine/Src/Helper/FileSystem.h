#pragma once
#include <filesystem>

namespace Dive
{
	namespace Helper
	{
		class FileSystem
		{
		public:
			static std::string GetDirectory(const std::string& filepath);
			static std::string GetFilepathWithoutExtension(const std::string& filepath);
			static std::string GetFileName(const std::string& filepath);
			static std::string GetFileNameWithoutExtension(const std::string& filepath);

			static std::wstring StringToWString(const std::string& str);
		};
	}
}
