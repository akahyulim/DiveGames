#include "DivePch.h"
#include "Settings.h"
#include "FileSystemHelper.h"

namespace Dive
{
	const std::string INI_FILE_NAME = "Dive.ini";// FileSystemHelper::GetCurrentDir() + "\\Dive.ini";
	const unsigned int DEFAULT_SCREEN_MODE = 0;
	const unsigned int DEFAULT_FRAME_RATE = 0;
	const bool DEFAULT_VSYNC = true;

	void Settings::Initialize()
	{
		if (!Load())
			Save();
	}

	bool Settings::Save()
	{
		//WritePrivateProfileStringA("Display", "nResolutionHeight", "900", INI_FILE_NAME.c_str());
		WritePrivateProfileStringA("Display", "nResolutionWidth", "1600", INI_FILE_NAME.c_str());
		WritePrivateProfileStringA("Display", "nResolutionHeight", "900", INI_FILE_NAME.c_str());
		WritePrivateProfileStringA("Display", "bVSync", "true", INI_FILE_NAME.c_str());

		return true;
	}
	
	bool Settings::Load()
	{
		//if(!FileSystemHelper::FileExists(INI_FILE_NAME))
		//	return false;

		GetPrivateProfileIntA("Display", "nResolutionWidth", 800, INI_FILE_NAME.c_str());
		GetPrivateProfileIntA("Display", "nResolutionHeight", 600, INI_FILE_NAME.c_str());


		return true;
	}
}
