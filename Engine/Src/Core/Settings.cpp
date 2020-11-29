#include "DivePch.h"
#include "Settings.h"
#include "FileSystemHelper.h"
#include "IniHelper.h"

namespace Dive
{
	Settings::Settings()
	{
		m_fileName = Dir::GetCurrentA() + "dive_settings.ini";
		m_screenMode = eScreenMode::Windowed;
		m_resolutionWidth = 800;
		m_resolutionHeight = 600;
		m_bVSync = false;
	}

	void Settings::Initialize(const char* fileName)
	{
		if (fileName)
			m_fileName = fileName;

		if (!Load())
			Save();
	}

	void Settings::Save()
	{
		IniHelper ih(m_fileName);
		ih["Display"]["nScreenMode"] = static_cast<unsigned int>(m_screenMode);
		ih["Display"]["nResolutionWidth"] = m_resolutionWidth;
		ih["Display"]["nResolutionHeight"] = m_resolutionHeight;
		ih["Display"]["bVSync"] = m_bVSync;
	}
	
	bool Settings::Load()
	{
		if (!File::ExistsA(m_fileName))
			return false;

		IniHelper ih(m_fileName);
		unsigned screenMode = ih["Display"]["nScreenMode"];
		m_screenMode = static_cast<eScreenMode>(screenMode);
		m_resolutionWidth = ih["Display"]["nResolutionWidth"];
		m_resolutionHeight = ih["Display"]["nResolutionHeight"];
		m_bVSync = ih["Display"]["bVSync"];

		return true;
	}
}
