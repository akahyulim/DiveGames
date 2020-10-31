/*
	- wstring를 사용하면 유니코드 문제가 발생할 수 있다.

	제작: 서보윤( akahyulim@gmail.com )
*/
#pragma once

namespace Dive
{
	namespace FileSystemHelper
	{
		//= DIRECTORIES ===================================================
		bool CreateDirectory_(const std::wstring& path);
		bool DeleteDirectory(const std::wstring& path);
		bool DirectoryExists(const std::wstring& path);
		bool IsDirectory(const std::wstring& path);

		//= FILES =========================================================
		bool FileExists(const std::wstring& name);
		bool RenameFile(const std::wstring& oldname, const std::wstring& newname);
		bool DeleteFile_(const std::wstring& name);
		bool CopyFile_(const std::wstring& source, const std::wstring& dest);

		//= PATH PARSING ==================================================
		std::wstring GetFilename(const std::wstring& filepath);
		std::wstring GetFilenameWithoutExtension(const std::wstring& filepath);
		std::wstring GetFilePathWithoutExtension(const std::wstring& filepath);
		std::wstring GetExtension(const std::wstring& filepath);

		std::wstring GetDirectory(const std::wstring& filepath);
		std::wstring GetWorkingDirectory();
		std::wstring GetParentDirectory(const std::wstring& path);
		std::wstring GetRelativeFilePath(const std::wstring& path);

		std::vector<std::wstring> GetDirectories(const std::wstring& path);
		std::vector<std::wstring> GetFiles(const std::wstring& path);
	}
}