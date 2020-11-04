/*
	FileSystem 정의부

	제작: 서보윤( akahyulim@gmail.com )
*/

//= INCLUDES ====================
#include "DivePch.h"
#include "FileSystemHelper.h"
#include "DiveDefs.h"
#include "Log.h"
//===============================


namespace Dive
{
	namespace FileSystemHelper
	{
		bool CreateDirectory_(const std::wstring& path)
		{
			try
			{
				return std::filesystem::create_directory(path);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("FileSystemHelper::CreateDirectory>> {:s}", e.what());
				return false;
			} 
		}
		
		bool DeleteDirectory(const std::wstring& path)
		{
			try
			{
				return std::filesystem::remove_all(path);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("FileSystemHelper::DeleteDirectory>> {:s}", e.what());
				return false;
			}
		}

		bool DirectoryExists(const std::wstring& path)
		{
			try
			{
				return std::filesystem::exists(path);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("FileSystemHelper::DirectoryExists>> {:s}", e.what());
				return false;
			}
		}

		bool IsDirectory(const std::wstring& path)
		{
			try
			{
				return std::filesystem::is_directory(path);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("FileSystemHelper::IsDirectory>> {:s}", e.what());
				return false;
			}
		}
		
		bool FileExists(const std::wstring& name)
		{
			try
			{
				return std::filesystem::exists(name);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("FileSystemHelper::FileExists>> {:s}", e.what());
				return false;
			}
		}

		bool RenameFile(const std::wstring & oldname, const std::wstring& newname)
		{
			if (!std::filesystem::is_regular_file(oldname))
				return false;

			try
			{
				return _wrename(oldname.c_str(), newname.c_str()) == 0;
			}
			catch(std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("FileSystemHelper::RenameFile>> {:s}", e.what());
				return false;
			}
		}

		bool DeleteFile_(const std::wstring& name)
		{
			if (!std::filesystem::is_regular_file(name))
				return false;

			try
			{
				return _wremove(name.c_str()) == 0;
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("FileSystemHelper::DeleteFile>> {:s}", e.what());
				return false;
			}
		}

		bool CopyFile_(const std::wstring& source, const std::wstring& dest)
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
				CORE_ERROR("FileSystemHelper::CopyFile>> {:s}", e.what());
				return false;
			}
		}
	
		std::wstring GetFilename(const std::wstring& filepath)
		{
			auto lastIndex = filepath.find_last_of(L"\\/");
			auto filename = filepath.substr(lastIndex + 1, filepath.length());

			return filename;
		}

		std::wstring GetFilenameWithoutExtension(const std::wstring& filepath)
		{
			auto filename = GetFilename(filepath);
			auto lastIndex = filename.find_last_of('.');
			auto name = filename.substr(0, lastIndex);

			return name;
		}

		std::wstring GetFilePathWithoutExtension(const std::wstring& filepath)
		{
			auto lastIndex = filepath.find_last_of('.');
			auto path = filepath.substr(0, lastIndex);

			return path;
		}

		std::wstring GetExtension(const std::wstring& filepath)
		{
			auto lastIndex = filepath.find_last_of('.');
			if (std::wstring::npos != lastIndex)
			{
				return filepath.substr(lastIndex, filepath.length());
			} 

			return L"";
		}

		std::wstring GetDirectory(const std::wstring& filepath)
		{
			auto lastIndex = filepath.find_last_of(L"\\/");
			auto dir = filepath.substr(0, lastIndex + 1);
			
			return dir;
		}

		std::wstring GetWorkingDirectory()
		{
			return std::filesystem::current_path().generic_wstring() + L"/";
		}

		std::wstring GetParentDirectory(const std::wstring& path)
		{
			auto result = path;
			auto found = path.find_last_of(L"/\\");

			if (found == std::wstring::npos)
				return path;

			if (found == path.length() - 1)
			{
				result = result.substr(0, found - 1);
				return GetParentDirectory(result);
			}
			
			return result.substr(0, found) + L"/";
		}

		//=============================================================================================
		// 현재 디렉토리와 root path가 다르다면 인자의 경로 리턴합니다.
		//=============================================================================================
		std::wstring GetRelativeFilePath(const std::wstring& path)
		{
			std::filesystem::path absWorkingDir = std::filesystem::absolute(GetWorkingDirectory());
			std::filesystem::path absPath = std::filesystem::absolute(path);

			if (absWorkingDir.root_path() != absPath.root_path())
				return absPath.generic_wstring();

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

			return relative.generic_wstring();
		}

		std::vector<std::wstring> GetDirectories(const std::wstring& path)
		{
			std::vector<std::wstring> dirs;

			std::filesystem::directory_iterator itEnd;
			for (std::filesystem::directory_iterator it(path); it != itEnd; it++)
			{
				if (!std::filesystem::is_directory(it->status()))
					continue;

				dirs.emplace_back(it->path().generic_wstring());
			}

			return dirs;
		}

		std::vector<std::wstring> GetFiles(const std::wstring& path)
		{
			std::vector<std::wstring> files;

			std::filesystem::directory_iterator itEnd;
			for (std::filesystem::directory_iterator it(path); it != itEnd; it++)
			{
				if (!std::filesystem::is_regular_file(it->status()))
					continue;

				files.emplace_back(it->path().generic_wstring());
			}

			return files;
		}

		bool DeleteFileA(const char* name)
		{
			if (!std::filesystem::is_regular_file(name))
				return false;

			try
			{
				return remove(name) == 0;
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("FileSystemHelper::DeleteFileA>> {:s}", e.what());
				return false;
			}
			return true;
		}

		std::string GetWorkingDirectoryA()
		{
			return std::filesystem::current_path().generic_string() + "/";
		}
	}
}