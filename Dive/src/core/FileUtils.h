#pragma once

namespace Dive
{
	class FileUtils
	{
	public:
		static std::filesystem::path OpenFile(const char* filter, HWND owner = nullptr, LPCSTR dir = nullptr);
		static std::filesystem::path SaveAs(const std::filesystem::path& oldFilePath, const char* filter, HWND owner = nullptr);

		static std::string SelectFolder();
	};
}
