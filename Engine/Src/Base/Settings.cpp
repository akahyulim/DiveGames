#include "divepch.h"
#include "Settings.h"
#include "IniHelper.h"
#include "Log.h"

namespace Dive
{
	void Settings::Initialize(const std::string& title)
	{
		m_FileName = std::filesystem::current_path().string() + '/' + title + ".ini";

		Load();

		CORE_INFO("Application Setting Informations");
		CORE_INFO("Application Title: {:s}", title);
		CORE_INFO("Window Size: {0:d}x{1:d}", m_Width, m_Height);
		CORE_INFO("Window Mode: {:s}", GetWindowMode() == eWindowModes::Windowed ? "Windowed" : (GetWindowMode() == eWindowModes::Borderless ? "Borderless" : "FullScreen"));
	}

	void Settings::Save()
	{
		IniHelper ini(m_FileName);

		ini["Window"]["Width"]			= static_cast<unsigned int>(m_Width);
		ini["Window"]["Height"]			= static_cast<unsigned int>(m_Height);
		ini["Window"]["bMaximize"]		= m_bMaximize;
		ini["Window"]["bFullScreen"]	= m_bFullScreen;
		ini["Window"]["bBorderless"]	= m_bBorderless;
	}

	void Settings::Load()
	{
		if (!std::filesystem::exists(m_FileName))
			return;

		IniHelper ini(m_FileName);

		m_Width			= static_cast<uint32_t>(ini["Window"]["Width"]);
		m_Height		= static_cast<uint32_t>(ini["Window"]["Height"]);
		m_bMaximize		= ini["Window"]["bMaximize"];
		m_bFullScreen	= ini["Window"]["bFullScreen"];
		m_bBorderless	= ini["Window"]["bBorderless"];
	}

	void Settings::SetWidth(uint32_t width)
	{
		if ((width <= 0) || (m_Width == width))
			return;

		m_Width = width;
	}

	void Settings::SetHeight(uint32_t height)
	{
		if ((height <= 0) || (m_Height == height))
			return;

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