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
	namespace File
	{
		bool ExistsA(const std::string& name)
		{
			try
			{
				return std::filesystem::exists(name);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("File::ExistsA>> {:s}", e.what());
				return false;
			}
		}

		bool DeleteA(const std::string& name)
		{
			if (!std::filesystem::is_regular_file(name))
				return false;

			try
			{
				return remove(name.c_str()) == 0;
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("File::DeleteA>> {:s}", e.what());
				return false;
			}
		}

		bool ExistsW(const std::wstring& name)
		{
			try
			{
				return std::filesystem::exists(name);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("File::ExistsW>> {:s}", e.what());
				return false;
			}
		}

		bool DeleteW(const std::wstring& name)
		{
			if (!std::filesystem::is_regular_file(name))
				return false;

			try
			{
				return _wremove(name.c_str()) == 0;
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("File::DeleteW>> {:s}", e.what());
				return false;
			}
		}
	}

	namespace Dir
	{
		std::string GetCurrentA()
		{
			return std::filesystem::current_path().generic_string() + "/";
		}

		std::wstring GetCurrentW()
		{
			return std::filesystem::current_path().generic_wstring() + L"/";
		}
	}
}