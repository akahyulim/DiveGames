#include "divepch.h"
#include "FileSystem.h"
#include "Log.h"
#include "Core/Context.h"

namespace Dive
{
	FileSystem::FileSystem(Context* pContext)
		: Object(pContext)
	{
	}

	FileSystem::~FileSystem()
	{
	}
	
	std::string ToUpperCase(const std::string& str)
	{
		auto copy = str;
		for (int i = 0; i < (int)copy.size(); ++i)
		{
			if ('a' <= copy[i] && 'z' >= copy[i])
			{
				copy[i] -= 32;
			}
		}

		return copy;
	}

	std::string ToLowerCase(const std::string& str)
	{
		auto copy = str;
		for (int i = 0; i < (int)copy.size(); ++i)
		{
			if ('A' <= copy[i] && 'Z' >= copy[i])
				copy[i] += 32;
		}

		return copy;
	}

	std::string StringLeftTrim(const std::string& str, const char* pTarget)
	{
		auto copy = str;
		copy.erase(0, copy.find_first_not_of(pTarget));

		return copy;
	}

	std::string StringRightTrim(const std::string& str, const char* pTarget)
	{
		auto copy = str;
		copy.erase(copy.find_last_not_of(pTarget) + 1);

		return copy;
	}

	std::string StringTrim(const std::string& str, const char* pTarget)
	{
		return StringLeftTrim(StringRightTrim(str, pTarget), pTarget);
	}

	std::string GetInternalPath(const std::string& fullPath)
	{
		auto copy = fullPath;
		std::string target = "\\";
		std::string replace = "/";
		size_t pos = 0;

		while ((pos = copy.find(target)) != std::string::npos)
		{
			copy.replace(pos, target.size(), replace);
		}
		
		return copy;
	}

	// 이게 필요한가....?
	std::string GetNativePath(const std::string& fullPath)
	{
		auto copy = fullPath;
		std::string target = "/";
		std::string replace = "\\";
		size_t pos = 0;

		while ((pos = copy.find(target)) != std::string::npos)
		{
			copy.replace(pos, target.size(), replace);
		}

		return copy;
	}

	void SplitPath(const std::string& fullPath, std::string& outPath, std::string& outFile, std::string& outExtension, bool bLowercaseExtension)
	{
		auto copy = GetInternalPath(fullPath);

		auto extensionPos = copy.find_last_of('.');
		auto pathPos = copy.find_last_of('/');

		if (extensionPos != std::string::npos && (pathPos != std::string::npos || extensionPos > pathPos))
		{
			// .을 포함한다.
			outExtension = copy.substr(extensionPos);
			if (bLowercaseExtension)
				outExtension = ToLowerCase(outExtension);

			copy = copy.substr(0, extensionPos);
		}
		else
			outExtension.clear();

		pathPos = copy.find_last_of('/');
		if (pathPos != std::string::npos)
		{
			outFile = copy.substr(pathPos + 1);
			outPath = copy.substr(0, pathPos + 1);
		}
		else
		{
			outFile = copy;
			outPath.clear();
		}
	}

	std::string GetPath(const std::string& fullPath)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension);

		return path;
	}
	
	std::string GetFileName(const std::string& fullPath)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension);

		return file;
	}
	
	std::string GetExtension(const std::string& fullPath, bool bLowercase)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension);

		return extension;
	}

	std::string GetFileNameAndExtension(const std::string& fullPath, bool bLowercaseExtension)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension, bLowercaseExtension);

		return file + extension;
	}
	
	std::string GetParentPath(const std::string& fullPath)
	{
		auto copy = GetInternalPath(fullPath);
		if (copy.find_last_of('/') != std::string::npos)
		{
			copy = copy.substr(0, copy.find_last_of('/'));
		
			if (copy.find_last_of('/') != std::string::npos)
			{
				return copy.substr(0, copy.find_last_of('/') + 1);
			}
		}

		return std::string();
	}
	
	bool IsAbsolutePath(const std::string& fullPath)
	{
		if(fullPath.empty())
			return false;

		auto path = GetInternalPath(fullPath);
		if (isalpha(path[0]) != 0 && path[1] == ':')
			return true;

		return false;
	}
}