#include "divepch.h"
#include "FileSystem.h"
#include "Base/Log.h"

#include <filesystem>


namespace Dive::Util::FileSystem
{
	bool CreateDirectory_(const std::string& path)
	{
		try
		{
			return std::filesystem::create_directory(path);
		}
		catch (std::filesystem::filesystem_error& e)
		{
			CORE_ERROR("CreateDirectory: {0:s}. {1:s}", e.what(), path);
			return false;
		} 
	}

	bool DeleteDirectory(const std::string& path)
	{
		try
		{
			return std::filesystem::remove_all(path);
		}
		catch (std::filesystem::filesystem_error& e)
		{
			CORE_ERROR("DeleteDirectory: {0:s}. {1:s}", e.what(), path);
			return false;
		}
	}

	bool DirectoryExists(const std::string& path)
	{
		try
		{
			return std::filesystem::exists(path);
		}
		catch (std::filesystem::filesystem_error& e)
		{
			CORE_ERROR("DirectoryExists: {0:s}. {1:s}", e.what(), path);
			return false;
		}
	}

	bool IsDirectory(const std::string& path)
	{
		try
		{
			return std::filesystem::is_directory(path);
		}
		catch (std::filesystem::filesystem_error& e)
		{
			CORE_ERROR("IsDirectory: {0:s}. {1:s}", e.what(), path);
			return false;
		}
	}

	bool FileExists(const std::string& filepath)
	{
		try
		{
			return std::filesystem::exists(filepath);
		}
		catch (std::filesystem::filesystem_error& e)
		{
			CORE_ERROR("FileExists: {0:s}. {1:s}", e.what(), filepath);
			return false;
		}
	}

	bool DeleteFile_(const std::string& filepath)
	{
		if (!std::filesystem::is_regular_file(filepath))
			return false;

		try
		{
			return remove(filepath.c_str()) == 0;
		}
		catch (std::filesystem::filesystem_error& e)
		{
			CORE_ERROR("DeleteFile: {0:s}. {1:s}", e.what(), filepath);
			return false;
		}
	}

	bool CopyFile_(const std::string& source, const std::string& dest)
	{
		if (source == dest)
			return false;

		if (!DirectoryExists(GetDirectory(dest)))
		{
			CreateDirectory_(GetDirectory(dest));
		}

		try
		{
			return std::filesystem::copy_file(source, dest, std::filesystem::copy_options::overwrite_existing);
		}
		catch (std::filesystem::filesystem_error& e)
		{
			CORE_ERROR("CopyFile: {0:s}. {1:s}", e.what(), source);
			return false;
		}
	}

	std::string GetFilename(const std::string& filepath)
	{
		auto lastIndex = filepath.find_last_of("\\/");
		auto filename = filepath.substr(lastIndex + 1, filepath.length());

		return filename;
	}

	std::string GetFilenameWithoutExtension(const std::string& filepath)
	{
		auto filename = GetFilename(filepath);
		auto lastIndex = filename.find_last_of('.');
		auto name = filename.substr(0, lastIndex);

		return name;
	}

	std::string GetFilePathWithoutExtension(const std::string& filepath)
	{
		auto lastIndex = filepath.find_last_of('.');
		auto path = filepath.substr(0, lastIndex);

		return path;
	}

	std::string GetExtension(const std::string& filepath)
	{
		auto lastIndex = filepath.find_last_of('.');
		if (std::string::npos != lastIndex)
		{
			return filepath.substr(lastIndex, filepath.length());
		} 

		return "";
	}

	std::string GetDirectory(const std::string& filepath)
	{
		auto lastIndex = filepath.find_last_of("\\/");
		auto dir = filepath.substr(0, lastIndex + 1);
			
		return dir;
	}

	std::string GetWorkingDirectory()
	{
		return std::filesystem::current_path().generic_string() + "/";
	}

	std::string GetParentDirectory(const std::string& path)
	{
		auto result = path;
		auto found = path.find_last_of("/\\");

		if (found == std::string::npos)
			return path;

		if (found == path.length() - 1)
		{
			result = result.substr(0, found - 1);
			return GetParentDirectory(result);
		}
			
		return result.substr(0, found) + "/";
	}

	//=============================================================================================
	// 현재 디렉토리와 root path가 다르다면 인자의 경로 리턴합니다.
	//=============================================================================================
	std::string GetRelativeFilePath(const std::string& path)
	{
		std::filesystem::path absWorkingDir = std::filesystem::absolute(GetWorkingDirectory());
		std::filesystem::path absPath = std::filesystem::absolute(path);

		if (absWorkingDir.root_path() != absPath.root_path())
			return absPath.generic_string();

		std::filesystem::path::const_iterator itAbsWorkingDir = absWorkingDir.begin();
		std::filesystem::path::const_iterator itAbsPath = absPath.begin();

		while ((*itAbsWorkingDir == *itAbsPath) && (itAbsWorkingDir != absWorkingDir.end()) && (itAbsPath != absPath.end()))
		{
			++itAbsWorkingDir;
			++itAbsPath;
		}

		std::filesystem::path relative;

		if (itAbsWorkingDir != absWorkingDir.end())
		{
			++itAbsWorkingDir;
			while (itAbsWorkingDir != absWorkingDir.end())
			{
				relative /= "..";
				++itAbsWorkingDir;
			}
		}

		while (itAbsPath != absPath.end())
		{
			relative /= *itAbsPath;
			++itAbsPath;
		}

		return relative.generic_string();
	}

	std::vector<std::string> GetDirectories(const std::string& path)
	{
		std::vector<std::string> dirs;

		std::filesystem::directory_iterator itEnd;
		for (std::filesystem::directory_iterator it(path); it != itEnd; it++)
		{
			if (!std::filesystem::is_directory(it->status()))
				continue;

			dirs.emplace_back(it->path().generic_string());
		}

		return dirs;
	}

	std::vector<std::string> GetFiles(const std::string& path)
	{
		std::vector<std::string> files;

		std::filesystem::directory_iterator itEnd;
		for (std::filesystem::directory_iterator it(path); it != itEnd; it++)
		{
			if (!std::filesystem::is_regular_file(it->status()))
				continue;

			files.emplace_back(it->path().generic_string());
		}

		return files;
	}
}