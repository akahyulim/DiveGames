/*
*/
#include "divepch.h"
#include "FormatHelper.h"

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
		// 일단 전부 소문자로 가정...
		std::string fileExtension = std::filesystem::path(filepath).extension().string();
		
		for (const auto& format : m_SupportedImageFormats)
		{
			if (fileExtension == format)
				return true;
		}

		return false;
	}

	bool FormatHelper::IsSupportedShaderFile(const std::string & path)
	{
		// 여기도 소문자로 가정
		std::string fileExtension = std::filesystem::path(path).extension().string();

		for (const auto& format : m_SupportedShaderFormats)
		{
			if (fileExtension == format)
				return true;
		}

		return false;
	}
	
	bool FormatHelper::IsEngineSceneFile(const std::string & path)
	{
		return std::filesystem::path(path).extension().string() == DEE_SCENE;
	}
	
	bool FormatHelper::IsEngineMaterialFile(const std::string & path)
	{
		return std::filesystem::path(path).extension().string() == DEE_MATERIAL;
	}
	
	bool FormatHelper::IsEngineTextureFile(const std::string & path)
	{
		return std::filesystem::path(path).extension().string() == DEE_TEXTURE;
	}
	
	bool FormatHelper::IsEngineMeshFile(const std::string & path)
	{
		return std::filesystem::path(path).extension().string() == DEE_MESH;
	}

	bool FormatHelper::IsEngineModelFile(const std::string & path)
	{
		return std::filesystem::path(path).extension().string() == DEE_MODEL;
	}
}