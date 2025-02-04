#pragma once

namespace Dive
{
	class FileUtils
	{
	public:
		static std::filesystem::path OpenFile(const char* pFilter, HWND owner = NULL, LPCSTR pDir = NULL);
		static std::filesystem::path SaveAs(const std::filesystem::path& oldFilePath, const char* pFilter, HWND owner = NULL);

		static std::string SelectFolder();
	};
}