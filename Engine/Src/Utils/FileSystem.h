#pragma once

// 단순 랩핑 함수들은 제거하자.
// Spratan은 String과 FileSystem 그리고 EngineFormat이 FileSystem이라는 Class로 묶여 있다.
namespace Dive::Util::FileSystem
{
	//= DIRECTORIES ===================================================
	bool CreateDirectory_(const std::string& path);
	bool DeleteDirectory(const std::string& path);
	bool DirectoryExists(const std::string& path);
	bool IsDirectory(const std::string& path);

	//= FILES =========================================================
	bool FileExists(const std::string& filepath);
	bool DeleteFile_(const std::string& filepath);
	bool CopyFile_(const std::string& source, const std::string& dest);

	//= PATH PARSING ==================================================
	std::string GetFilename(const std::string& filepath);
	std::string GetFilenameWithoutExtension(const std::string& filepath);
	std::string GetFilePathWithoutExtension(const std::string& filepath);
	std::string GetExtension(const std::string& filepath);

	std::string GetDirectory(const std::string& filepath);
	std::string GetWorkingDirectory();
	std::string GetParentDirectory(const std::string& path);
	std::string GetRelativeFilePath(const std::string& path);

	std::vector<std::string> GetDirectories(const std::string& path);
	std::vector<std::string> GetFiles(const std::string& path);
}