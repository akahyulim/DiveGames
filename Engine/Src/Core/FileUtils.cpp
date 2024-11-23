#include "divepch.h"
#include "FileUtils.h"
#include "Log.h"
#include "StringUtils.h"

namespace Dive
{
	std::string FileUtils::GetCurrentDir()
	{
		auto ret = std::filesystem::current_path().string();
		ret = GetInternalPath(ret);

		return StringUtils::AddTrailingSlash(ret);
	}

	void FileUtils::SetCurrentDir(const std::string& pathName)
	{
		std::filesystem::current_path(pathName);
	}

	std::string FileUtils::GetProgramDir()
	{
		char exeName[MAX_PATH] = { 0, };
		GetModuleFileNameA(nullptr, exeName, MAX_PATH);

		return GetInternalPath(exeName);
	}

	std::string FileUtils::GetInternalPath(const std::string& fullPath)
	{
		auto copy = fullPath;
		std::string pTarget = "\\";
		std::string replace = "/";
		size_t pos = 0;

		while ((pos = copy.find(pTarget)) != std::string::npos)
		{
			copy.replace(pos, pTarget.size(), replace);
		}

		return copy;
	}

	// 이게 필요한가....?
	std::string FileUtils::GetNativePath(const std::string& fullPath)
	{
		auto copy = fullPath;
		std::string pTarget = "/";
		std::string replace = "\\";
		size_t pos = 0;

		while ((pos = copy.find(pTarget)) != std::string::npos)
		{
			copy.replace(pos, pTarget.size(), replace);
		}

		return copy;
	}

	void FileUtils::SplitPath(const std::string& fullPath, std::string& outPath, std::string& outFile, std::string& outExtension, bool bLowercaseExtension)
	{
		auto copy = GetInternalPath(fullPath);

		auto extensionPos = copy.find_last_of('.');
		auto pathPos = copy.find_last_of('/');

		if (extensionPos != std::string::npos && (pathPos != std::string::npos || extensionPos > pathPos))
		{
			// .을 포함한다.
			outExtension = copy.substr(extensionPos);
			if (bLowercaseExtension)
				outExtension = StringUtils::ToLowerCase(outExtension);

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

	std::string FileUtils::GetPath(const std::string& fullPath)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension);

		return path;
	}

	std::string FileUtils::GetLastPath(const std::string& fullPath)
	{
		std::filesystem::path fixedPath = fullPath;

		return GetInternalPath(fixedPath.parent_path().filename().string());
	}

	std::string FileUtils::GetFileName(const std::string& fullPath)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension);

		return file;
	}

	std::string FileUtils::GetExtension(const std::string& fullPath, bool bLowercase)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension);

		return extension;
	}

	std::string FileUtils::GetFileNameAndExtension(const std::string& fullPath, bool bLowercaseExtension)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension, bLowercaseExtension);

		return file + extension;
	}

	std::string FileUtils::GetParentPath(const std::string& fullPath)
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

	bool FileUtils::IsAbsolutePath(const std::string& fullPath)
	{
		if (fullPath.empty())
			return false;

		auto path = GetInternalPath(fullPath);
		if (isalpha(path[0]) != 0 && path[1] == ':')
			return true;

		return false;
	}

	bool FileUtils::FileExists(const std::string& fileName)
	{
		return std::filesystem::exists(fileName);
	}

	bool FileUtils::DirExists(const std::string& pathName)
	{
		return std::filesystem::exists(pathName) && std::filesystem::is_directory(pathName);
	}

	bool FileUtils::CreateDir(const std::string& pathName)
	{
		return std::filesystem::create_directories(pathName);
	}

	bool FileUtils::Copy(const std::string& sourceFileName, const std::string& destFileName)
	{
		if (!FileExists(sourceFileName))
		{
			DV_LOG(FileUtils, err, "원본 파일({:s})이 존재하지 않아 복사를 수행할 수 없습니다.", sourceFileName);
			return false;
		}

		if (sourceFileName == destFileName)
		{
			DV_LOG(FileUtils, warn, "동일한 파일 이름을 전달받아 복사를 수행할 수 없습니다. : {0:s} == {:1s}",
				sourceFileName, destFileName);
			return false;
		}

		// dest에 붙어있는 경로가 존재하지 않는다면 직접 생성한 후 수행
		if (!DirExists(GetPath(destFileName)))
		{
			CreateDir(GetPath(destFileName));
		}

		return std::filesystem::copy_file(sourceFileName, destFileName, std::filesystem::copy_options::overwrite_existing);
	}

	bool FileUtils::Rename(const std::string& sourceFileName, const std::string& destFileName)
	{
		if (!FileExists(sourceFileName))
		{
			DV_LOG(FileUtils, err, "원본 파일({:s})이 존재하지 않아 이름을 변경할 수 없습니다.", sourceFileName);
			return false;
		}

		if (destFileName.empty())
		{
			DV_LOG(FileUtils, err, "파일의 새로운 이름({:s})을 잘못 전달받아 변경할 수 없습니다.", destFileName);
			return false;
		}

		std::filesystem::rename(sourceFileName, destFileName);
		return true;
	}

	bool FileUtils::Delete(const std::string& fileName)
	{
		return std::filesystem::exists(fileName) && std::filesystem::remove(fileName);
	}
}