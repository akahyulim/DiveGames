/*
	
	¡¶¿€: º≠∫∏¿±( akahyulim@gmail.com )
*/
#pragma once

namespace Dive
{
	namespace File
	{
		bool IsExistFile(const std::string& name);
		bool DeleteA(const std::string& name);

		std::string GetCurrentA();
		std::string GetFileNameWithExtension(const std::string& filepath);
		std::string GetFileName(const std::string& filepath);
		std::string GetFileExtension(const std::string& filepath);


		std::wstring GetCurrentW();


		// engine format 
		bool IsEngineMeshFile(const std::string& filepath);
		bool IsEngineMaterialFile(const std::string& filepath);
		bool IsEngineTextureFile(const std::string& filepath);
		bool IsEngineSceneFile(const std::string& filepath);
		bool IsEngineFile(const std::string& filepath);
	}

	static const char* EXTENSION_MESH		= ".mesh";
	static const char* EXTENSION_MATERIAL	= ".material";
	static const char* EXTENSION_TEXTURE	= ".texture";
	static const char* EXTENSION_SCENE		= ".scene";
}