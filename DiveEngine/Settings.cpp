#include "Settings.h"
#include "IniHelper.h"
#include "FileSystemHelper.h"
#include "Log.h"

namespace Dive
{
	void Settings::Initialize(const std::string& title)
	{
		m_filename = FileSystemHelper::GetWorkingDirectory() + title + ".ini";

		Load();

		CORE_INFO("Application Setting Informations...");
		CORE_INFO("Application Title: {:s}", title);
		CORE_INFO("Window Resolution: {0:d}x{1:d}", m_width, m_height);
		CORE_INFO("Window Maximize: {:b}", m_bMaximize);
		CORE_INFO("Window FullScreen: {:b}", m_bFullScreen);
		CORE_INFO("Window Borderless: {:b}", m_bBorderless);
		CORE_INFO("Window Mode: {:s}", GetWindowMode() == eWindowModes::Windowed ? "Windowed" : (GetWindowMode() == eWindowModes::Borderless ? "Borderless" : "FullScreen"));
	}

	void Settings::Default()
	{
		// �̷��ϱ� �ڲ� ���ε��� �����̴�.
		// �׳� ��� ������ �ʱ�ȭ�ϴ°� ���� �� ����.
		IniHelper ini(m_filename);
		ini["Window"]["Width"] = 800;
		ini["Window"]["Height"] = 600;
		ini["Window"]["bMaximize"] = false;
		ini["Window"]["bFullScreen"] = false;
		ini["Window"]["bBorderless"] = false;

		Load();
	}

	void Settings::Save()
	{
		if (!FileSystemHelper::FileExists(m_filename))
			Default();

		IniHelper ini(m_filename);
		ini["Window"]["Width"] = m_width;
		ini["Window"]["Height"] = m_height;
		ini["Window"]["bMaximize"] = m_bMaximize;
		ini["Window"]["bFullScreen"] = m_bFullScreen;
		ini["Window"]["bBorderless"] = m_bBorderless;

		CORE_TRACE("ini������ �����մϴ�.");
	}

	void Settings::Load()
	{
		if (!FileSystemHelper::FileExists(m_filename))
			Default();

		IniHelper ini(m_filename);
		m_width = ini["Window"]["Width"];
		m_height = ini["Window"]["Height"];
		m_bMaximize = ini["Window"]["bMaximize"];
		m_bFullScreen = ini["Window"]["bFullScreen"];
		m_bBorderless = ini["Window"]["bBorderless"];
	}

	void Settings::SetWidth(unsigned int width)
	{
		if ((width <= 0) || (m_width == width))
		{
			CORE_ERROR("Settings::SetWidth >> ��ȿ���� ���� ũ��({:d})�� ������ �õ��Ͽ����ϴ�.", width);
		}

		m_width = width;
	}

	void Settings::SetHeight(unsigned int height)
	{
		if ((height <= 0) || (m_height == height))
		{
			CORE_ERROR("Settings::SetWidth >> ��ȿ���� ���� ũ��({:d})�� ������ �õ��Ͽ����ϴ�.", height);
		}

		m_width = height;
	}
	eWindowModes Settings::GetWindowMode()
	{
		if (m_bFullScreen)
		{
			m_windowMode = eWindowModes::FullScreen;
		}
		else if (m_bBorderless)
		{
			m_windowMode = eWindowModes::Borderless;
		}
		else
		{
			m_windowMode = eWindowModes::Windowed;
		}

		return m_windowMode;
	}

	void Settings::SetWindowMode(eWindowModes mode)
	{
		m_windowMode = mode;

		switch (m_windowMode)
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