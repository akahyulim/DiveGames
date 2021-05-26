/*
*/
#include "FormatHelper.h"
#include "StringHelper.h"
#include "FileSystemHelper.h"

namespace Dive
{
	std::vector<std::string> FormatHelper::m_supportedImageFormats;
	std::vector<std::string> FormatHelper::m_supportedShaderFormats;

	void FormatHelper::Initialize()
	{
		m_supportedImageFormats =
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

		m_supportedShaderFormats = 
		{	
			".hlsl"
		};
	}
	
	bool FormatHelper::IsSupportedImageFile(const std::string & filepath)
	{
		std::string fileExtension = FileSystemHelper::GetExtension(filepath);

		for (const auto& format : m_supportedImageFormats)
		{
			if ((fileExtension == format) || fileExtension == StringHelper::ToUppercase(format))
				return true;
		}

		return false;
	}

	bool FormatHelper::IsSupportedShaderFile(const std::string & path)
	{
		std::string fileExtension = FileSystemHelper::GetExtension(path);

		for (const auto& format : m_supportedShaderFormats)
		{
			if ((fileExtension == format) || (fileExtension == StringHelper::ToUppercase(format)))
				return true;
		}

		return false;
	}
	
	bool FormatHelper::IsEngineSceneFile(const std::string & path)
	{
		return FileSystemHelper::GetExtension(path) == DEE_SCENE;
	}
	
	bool FormatHelper::IsEngineMaterialFile(const std::string & path)
	{
		return FileSystemHelper::GetExtension(path) == DEE_MATERIAL;
	}
	
	bool FormatHelper::IsEngineTextureFile(const std::string & path)
	{
		return FileSystemHelper::GetExtension(path) == DEE_TEXTURE;
	}
	
	bool FormatHelper::IsEngineMeshFile(const std::string & path)
	{
		return FileSystemHelper::GetExtension(path) == DEE_MESH;
	}

	bool FormatHelper::IsEngineModelFile(const std::string & path)
	{
		return FileSystemHelper::GetExtension(path) == DEE_MODEL;
	}
}