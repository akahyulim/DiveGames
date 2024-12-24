#pragma once

namespace Dive
{
	class FileUtils
	{
	public:
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
}