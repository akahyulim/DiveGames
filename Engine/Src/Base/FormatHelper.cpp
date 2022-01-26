/*
*/
#include "divepch.h"
#include "FormatHelper.h"
#include "Utils/String.h"
#include "Utils/FileSystem.h"

namespace Dive
{
	std::vector<std::string> FormatHelper::m_SupportedImageFormats;
	std::vector<std::string> FormatHelper::m_SupportedShaderFormats;

	void FormatHelper::Initialize()
	{
		m_SupportedImageFormats =
		{
			".jpg",
			".png",
			".bmp",
			".tga",
			".dds",
			".exr",
			".raw",
			".gif",
			".hdr",
			".ico",
			".iff",
			".jng",
			".jpeg",
			".koala",
			".kodak",
			".mng",
			".pcx",
			".pbm",
			".pgm",
			".ppm",
			".pfm",
			".pict",
			".psd",
			".raw",
			".sgi",
			".targa",
			".tiff",
			".tif",
			".wbmp",
			".webp",
			".xbm",
			".xpm"
		};

		m_SupportedShaderFormats = 
		{	
			".hlsl"
		};
	}
	
	bool FormatHelper::IsSupportedImageFile(const std::string & filepath)
	{
		std::string fileExtension = Util::FileSystem::GetExtension(filepath);

		for (const auto& format : m_SupportedImageFormats)
		{
			if ((fileExtension == format) || fileExtension == Util::String::ToUppercase(format))
				return true;
		}

		return false;
	}

	bool FormatHelper::IsSupportedShaderFile(const std::string & path)
	{
		std::string fileExtension = Util::FileSystem::GetExtension(path);

		for (const auto& format : m_SupportedShaderFormats)
		{
			if ((fileExtension == format) || (fileExtension == Util::String::ToUppercase(format)))
				return true;
		}

		return false;
	}
	
	bool FormatHelper::IsEngineSceneFile(const std::string & path)
	{
		return Util::FileSystem::GetExtension(path) == DEE_SCENE;
	}
	
	bool FormatHelper::IsEngineMaterialFile(const std::string & path)
	{
		return Util::FileSystem::GetExtension(path) == DEE_MATERIAL;
	}
	
	bool FormatHelper::IsEngineTextureFile(const std::string & path)
	{
		return Util::FileSystem::GetExtension(path) == DEE_TEXTURE;
	}
	
	bool FormatHelper::IsEngineMeshFile(const std::string & path)
	{
		return Util::FileSystem::GetExtension(path) == DEE_MESH;
	}

	bool FormatHelper::IsEngineModelFile(const std::string & path)
	{
		return Util::FileSystem::GetExtension(path) == DEE_MODEL;
	}
}