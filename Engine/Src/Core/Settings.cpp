#include "DivePch.h"
#include "Settings.h"
#include "FileSystemHelper.h"
#include "IniHelper.h"

namespace Dive
{
	Settings::Settings()
	{
		m_FileName = Dir::GetCurrentA() + "dive_settings.ini";
		m_ScreenMode = eScreenMode::Windowed;
		m_ResolutionWidth = 800;
		m_ResolutionHeight = 600;
		m_bVSync = false;
	}

	void Settings::Initialize(const char* fileName)
	{
		if (fileName)
			m_FileName = fileName;

		if (!Load())
			Save();
	}

	void Settings::Save()
	{
		IniHelper ih(m_FileName);
		ih["Display"]["nScreenMode"] = static_cast<unsigned int>(m_ScreenMode);
		ih["Display"]["nResolutionWidth"] = m_ResolutionWidth;
		ih["Display"]["nResolutionHeight"] = m_ResolutionHeight;
		ih["Display"]["bVSync"] = m_bVSync;
	}
	
	bool Settings::Load()
	{
		if (!File::ExistsA(m_FileName))
			return false;

		IniHelper ih(m_FileName);
		unsigned screenMode = ih["Display"]["nScreenMode"];
		m_ScreenMode = static_cast<eScreenMode>(screenMode);
		m_ResolutionWidth = ih["Display"]["nResolutionWidth"];
		m_ResolutionHeight = ih["Display"]["nResolutionHeight"];
		m_bVSync = ih["Display"]["bVSync"];

		return true;
	}
}
