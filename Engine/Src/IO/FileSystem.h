#pragma once

namespace Dive
{
	// 이걸 왜 굳이 클래스로 묶었을까?
	class FileSystem
	{
	public:
		// file and directory
		static std::string GetCurrentDir();
		static void SetCurrentDir(const std::string& pathName);
		static std::string GetProgramDir();
		static std::string GetInternalPath(const std::string& fullPath);
		static std::string GetNativePath(const std::string& fullPath);
		static void SplitPath(const std::string& fullPath, std::string& outPath, std::string& outFile, std::string& outExtension, bool bLowercaseExtension = true);
		static std::string GetPath(const std::string& fullPath);
		static std::string GetLastPath(const std::string& fullPath);
		static std::string GetFileName(const std::string& fullPath);
		static std::string GetExtension(const std::string& fullPath, bool bLowercase = true);
		static std::string GetFileNameAndExtension(const std::string& fullPath, bool bLowercaseExtension = false);
		static std::string GetParentPath(const std::string& fullPath);
		static bool IsAbsolutePath(const std::string& fullPath);
		static bool FileExists(const std::string& fileName);
		static bool DirExists(const std::string& pathName);
		static bool CreateDir(const std::string& pathName);
		static bool Copy(const std::string& sourceFileName, const std::string& destFileName);
		static bool Rename(const std::string& sourceFileName, const std::string& destFileName);
		static bool Delete(const std::string& fileName);
	};

	// strings
	std::string ToUpperCase(const std::string& str);
	std::string ToLowerCase(const std::string& str);
	std::string StringLeftTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
	std::string StringRightTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
	std::string StringTrim(const std::string& str, const char* pTarget = " \t\n\r\f\v");
	std::string AddTrailingSlash(const std::string& pathName);
	std::string RemoveTrailingSlash(const std::string& pathName);
	std::string StringReplace(const std::string& str, const std::string& target, const std::string& replace);
	std::wstring StringToWstring(const std::string& str);
	std::string WstringToString(const std::wstring& str);
	std::vector<std::string> StringSplit(const std::string& str, char seperator);
}