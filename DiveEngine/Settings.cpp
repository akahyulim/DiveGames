#include "Settings.h"
#include "IniHelper.h"
#include "FileSystemHelper.h"
#include "Log.h"

namespace dive
{
	void Settings::Initialize(const std::string& title)
	{
		m_FileName = FileSystemHelper::GetWorkingDirectory() + title + ".ini";

		Load();

		CORE_INFO("Application Setting Informations...");
		CORE_INFO("Application Title: {:s}", title);
		CORE_INFO("Window Size: {0:d}x{1:d}", m_Width, m_Height);
		CORE_INFO("Window Maximize: {:b}", m_bMaximize);
		CORE_INFO("Window FullScreen: {:b}", m_bFullScreen);
		CORE_INFO("Window Borderless: {:b}", m_bBorderless);
		CORE_INFO("Window Mode: {:s}", GetWindowMode() == eWindowModes::Windowed ? "Windowed" : (GetWindowMode() == eWindowModes::Borderless ? "Borderless" : "FullScreen"));
	}

	void Settings::Default()
	{
		IniHelper ini(m_FileName);

		// Window
		ini["Window"]["Width"]			= m_Width		= 800;
		ini["Window"]["Height"]			= m_Height		= 600;
		ini["Window"]["bMaximize"]		= m_bMaximize	= false;
		ini["Window"]["bFullScreen"]	= m_bFullScreen = false;
		ini["Window"]["bBorderless"]	= m_bBorderless = false;
	}

	void Settings::Save()
	{
		if (!FileSystemHelper::FileExists(m_FileName))
			Default();

		IniHelper ini(m_FileName);

		// Window
		ini["Window"]["Width"] = m_Width;
		ini["Window"]["Height"] = m_Height;
		ini["Window"]["bMaximize"] = m_bMaximize;
		ini["Window"]["bFullScreen"] = m_bFullScreen;
		ini["Window"]["bBorderless"] = m_bBorderless;

		CORE_TRACE("ini파일을 저장합니다.");
	}

	void Settings::Load()
	{
		if (!FileSystemHelper::FileExists(m_FileName))
			Default();

		IniHelper ini(m_FileName);

		// Window
		m_Width = ini["Window"]["Width"];
		m_Height = ini["Window"]["Height"];
		m_bMaximize = ini["Window"]["bMaximize"];
		m_bFullScreen = ini["Window"]["bFullScreen"];
		m_bBorderless = ini["Window"]["bBorderless"];
	}

	void Settings::SetWidth(unsigned int width)
	{
		if ((width <= 0) || (m_Width == width))
		{
			CORE_ERROR("Settings::SetWidth >> 유효하지 않은 크기({:d})로 변경을 시도하였습니다.", width);
			return;
		}

		m_Width = width;
	}

	void Settings::SetHeight(unsigned int height)
	{
		if ((height <= 0) || (m_Height == height))
		{
			CORE_ERROR("Settings::SetHeight >> 유효하지 않은 크기({:d})로 변경을 시도하였습니다.", height);
			return;
		}

		m_Height = height;
	}
	eWindowModes Settings::GetWindowMode()
	{
		if (m_bFullScreen)
		{
			m_WindowMode = eWindowModes::FullScreen;
		}
		else if (m_bBorderless)
		{
			m_WindowMode = eWindowModes::Borderless;
		}
		else
		{
			m_WindowMode = eWindowModes::Windowed;
		}

		return m_WindowMode;
	}

	void Settings::SetWindowMode(eWindowModes mode)
	{
		m_WindowMode = mode;

		switch (m_WindowMode)
		{
		case eWindowModes::Windowed:
		{
			m_bBorderless = false;
			m_bFullScreen = false;
		}
		break;
		case eWindowModes::Borderless:
		{
			m_bBorderless = true;
			m_bFullScreen = false;
		}
		break;
		case eWindowModes::FullScreen:
		{
			m_bBorderless = true;
			m_bFullScreen = true;
		}
		break;
		default:
			return;
		}
	}
}