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

	// �̰� �ʿ��Ѱ�....?
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
			// .�� �����Ѵ�.
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
			DV_LOG(FileUtils, err, "���� ����({:s})�� �������� �ʾ� ���縦 ������ �� �����ϴ�.", sourceFileName);
			return false;
		}

		if (sourceFileName == destFileName)
		{
			DV_LOG(FileUtils, warn, "������ ���� �̸��� ���޹޾� ���縦 ������ �� �����ϴ�. : {0:s} == {:1s}",
				sourceFileName, destFileName);
			return false;
		}

		// dest�� �پ��ִ� ��ΰ� �������� �ʴ´ٸ� ���� ������ �� ����
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
			DV_LOG(FileUtils, err, "���� ����({:s})�� �������� �ʾ� �̸��� ������ �� �����ϴ�.", sourceFileName);
			return false;
		}

		if (destFileName.empty())
		{
			DV_LOG(FileUtils, err, "������ ���ο� �̸�({:s})�� �߸� ���޹޾� ������ �� �����ϴ�.", destFileName);
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