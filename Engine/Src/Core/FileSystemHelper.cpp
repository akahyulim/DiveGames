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
		bool IsExistFile(const std::string& name)
		{
			try
			{
				return std::filesystem::exists(name);
			}
			catch (std::filesystem::filesystem_error& e)
			{
				CORE_ERROR("File::IsExistFile>> {:s}", e.what());
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
		std::string GetCurrentA()
		{
			return std::filesystem::current_path().generic_string() + "/";
		}

		std::string GetFileNameWithExtension(const std::string & filepath)
		{
			return std::filesystem::path(filepath).filename().generic_string();
		}

		std::string GetFileName(const std::string & filepath)
		{
			auto filename	= GetFileNameWithExtension(filepath);
			auto index		= filename.find_last_of('.');

			if (index != std::string::npos)
				return filename.substr(0, index);

			return "";
		}

		std::string GetFileExtension(const std::string & filepath)
		{
			std::string extension;

			try
			{
				extension = std::filesystem::path(filepath).extension().generic_string();
			}
			catch(std::system_error& e)
			{
				CORE_ERROR("{:s}", e.what());
			}

			return extension;
		}

		std::wstring GetCurrentW()
		{
			return std::filesystem::current_path().generic_wstring() + L"/";
		}

		bool IsEngineMeshFile(const std::string & filepath)
		{
			return GetFileExtension(filepath) == EXTENSION_MESH;
		}

		bool IsEngineMaterialFile(const std::string & filepath)
		{
			return GetFileExtension(filepath) == EXTENSION_MATERIAL;
		}
		
		bool IsEngineTextureFile(const std::string & filepath)
		{
			return GetFileExtension(filepath) == EXTENSION_TEXTURE;
		}
		
		bool IsEngineSceneFile(const std::string & filepath)
		{
			return GetFileExtension(filepath) == EXTENSION_SCENE;
		}
		
		bool IsEngineFile(const std::string & filepath)
		{
			return	IsEngineMaterialFile(filepath)	||
					IsEngineTextureFile(filepath)	||
					IsEngineSceneFile(filepath);
		}
	}
}