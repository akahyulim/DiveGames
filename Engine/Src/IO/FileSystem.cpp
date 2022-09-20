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

	std::string FileSystem::ToUpperCase(const std::string& str)
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

	std::string FileSystem::ToLowerCase(const std::string& str)
	{
		auto copy = str;
		for (int i = 0; i < (int)copy.size(); ++i)
		{
			if ('A' <= copy[i] && 'Z' >= copy[i])
				copy[i] += 32;
		}

		return copy;
	}

	std::string FileSystem::StringLeftTrim(const std::string& str, const char* pTarget)
	{
		auto copy = str;
		copy.erase(0, copy.find_first_not_of(pTarget));

		return copy;
	}

	std::string FileSystem::StringRightTrim(const std::string& str, const char* pTarget)
	{
		auto copy = str;
		copy.erase(copy.find_last_not_of(pTarget) + 1);

		return copy;
	}

	std::string FileSystem::StringTrim(const std::string& str, const char* pTarget)
	{
		return StringLeftTrim(StringRightTrim(str, pTarget), pTarget);
	}

	std::string FileSystem::AddTrailingSlash(const std::string& pathName)
	{
		if (pathName.empty())
			return std::string();

		// \\를 모두 /로 변경
		auto ret = GetInternalPath(pathName);
		
		if (ret.back() != '/')
			ret += '/';

		return ret;
	}

	std::string FileSystem::RemoveTrailingSlash(const std::string& pathName)
	{
		if(pathName.empty())
			return std::string();

		auto ret = pathName;

		if (ret.back() == '/')
			ret.resize(ret.length() - 1);

		return ret;
	}

	std::wstring FileSystem::StringToWstring(const std::string& str)
	{
		if (str.empty())
			return std::wstring();

		auto size = ::MultiByteToWideChar(
			CP_ACP, 
			0, 
			str.c_str(), 
			static_cast<int>(str.size()), 
			nullptr, 
			0);
		
		std::wstring ret;
		ret.resize(size);

		::MultiByteToWideChar(
			CP_ACP,
			0,
			str.c_str(),
			static_cast<int>(str.size()),
			const_cast<wchar_t*>(ret.c_str()),
			static_cast<int>(ret.size()));

		return ret;
	}

	std::string FileSystem::WstringToString(const std::wstring& str)
	{
		if (str.empty())
			return std::string();

		auto size = ::WideCharToMultiByte(
			CP_ACP,
			0,
			str.c_str(),
			static_cast<int>(str.size()),
			nullptr,
			0,
			nullptr,
			nullptr);

		std::string ret;
		ret.resize(size);

		::WideCharToMultiByte(
			CP_ACP,
			0,
			str.c_str(),
			static_cast<int>(str.size()),
			const_cast<char*>(ret.c_str()),
			static_cast<int>(ret.size()),
			nullptr,
			nullptr);

		return ret;
	}

	std::string FileSystem::GetCurrentDir()
	{
		auto ret = std::filesystem::current_path().string();
		ret = GetInternalPath(ret);
		
		return AddTrailingSlash(ret);
	}

	void FileSystem::SetCurrentDir(const std::string& pathName)
	{
		std::filesystem::current_path(pathName);
	}

	std::string FileSystem::GetInternalPath(const std::string& fullPath)
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
	std::string FileSystem::GetNativePath(const std::string& fullPath)
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

	void FileSystem::SplitPath(const std::string& fullPath, std::string& outPath, std::string& outFile, std::string& outExtension, bool bLowercaseExtension)
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

	std::string FileSystem::GetPath(const std::string& fullPath)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension);

		return path;
	}

	std::string FileSystem::GetFileName(const std::string& fullPath)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension);

		return file;
	}

	std::string FileSystem::GetExtension(const std::string& fullPath, bool bLowercase)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension);

		return extension;
	}

	std::string FileSystem::GetFileNameAndExtension(const std::string& fullPath, bool bLowercaseExtension)
	{
		std::string path, file, extension;
		SplitPath(fullPath, path, file, extension, bLowercaseExtension);

		return file + extension;
	}

	std::string FileSystem::GetParentPath(const std::string& fullPath)
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

	bool FileSystem::IsAbsolutePath(const std::string& fullPath)
	{
		if (fullPath.empty())
			return false;

		auto path = GetInternalPath(fullPath);
		if (isalpha(path[0]) != 0 && path[1] == ':')
			return true;

		return false;
	}

	bool FileSystem::FileExists(const std::string& fileName)
	{
		return std::filesystem::exists(fileName);
	}

	bool FileSystem::DirExists(const std::string& pathName)
	{
		return std::filesystem::exists(pathName) && std::filesystem::is_directory(pathName);
	}

	bool FileSystem::CreateDir(const std::string& pathName)
	{
		return std::filesystem::create_directories(pathName);
	}

	bool FileSystem::Copy(const std::string& sourceFileName, const std::string& destFileName)
	{
		if (!FileExists(sourceFileName))
		{
			DV_LOG_ENGINE_ERROR("원본 파일({:s})이 존재하지 않아 복사를 수행할 수 없습니다.", sourceFileName);
			return false;
		}

		if (sourceFileName == destFileName)
		{
			DV_LOG_ENGINE_WARN("동일한 파일 이름을 전달받아 복사를 수행할 수 없습니다. : {0:s} == {:1s}",
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

	bool FileSystem::Rename(const std::string& sourceFileName, const std::string& destFileName)
	{
		if (!FileExists(sourceFileName))
		{
			DV_LOG_ENGINE_ERROR("원본 파일({:s})이 존재하지 않아 이름을 변경할 수 없습니다.", sourceFileName);
			return false;
		}

		if (destFileName.empty())
		{
			DV_LOG_ENGINE_ERROR("파일의 새로운 이름({:s})을 잘못 전달받아 변경할 수 없습니다.", destFileName);
			return false;
		}

		std::filesystem::rename(sourceFileName, destFileName);
		return true;
	}

	bool FileSystem::Delete(const std::string& fileName)
	{
		return std::filesystem::exists(fileName) && std::filesystem::remove(fileName);
	}
}