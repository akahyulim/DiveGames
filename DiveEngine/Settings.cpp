#include "Settings.h"
#include "IniHelper.h"
#include "FileSystemHelper.h"
#include "Log.h"

namespace dive
{
	void Settings::Initialize(const std::string& title)
	{
		mFileName = FileSystemHelper::GetWorkingDirectory() + title + ".ini";

		Load();

		CORE_INFO("Application Setting Informations...");
		CORE_INFO("Application Title: {:s}", title);
		CORE_INFO("Window Size: {0:d}x{1:d}", mWidth, mHeight);
		CORE_INFO("Window Maximize: {:b}", mbMaximize);
		CORE_INFO("Window FullScreen: {:b}", mbFullScreen);
		CORE_INFO("Window Borderless: {:b}", mbBorderless);
		CORE_INFO("Window Mode: {:s}", GetWindowMode() == eWindowModes::Windowed ? "Windowed" : (GetWindowMode() == eWindowModes::Borderless ? "Borderless" : "FullScreen"));
	}

	void Settings::Default()
	{
		IniHelper ini(mFileName);

		// Window
		ini["Window"]["Width"]			= mWidth		= 800;
		ini["Window"]["Height"]			= mHeight		= 600;
		ini["Window"]["bMaximize"]		= mbMaximize	= false;
		ini["Window"]["bFullScreen"]	= mbFullScreen = false;
		ini["Window"]["bBorderless"]	= mbBorderless = false;
	}

	void Settings::Save()
	{
		if (!FileSystemHelper::FileExists(mFileName))
			Default();

		IniHelper ini(mFileName);

		// Window
		ini["Window"]["Width"] = mWidth;
		ini["Window"]["Height"] = mHeight;
		ini["Window"]["bMaximize"] = mbMaximize;
		ini["Window"]["bFullScreen"] = mbFullScreen;
		ini["Window"]["bBorderless"] = mbBorderless;

		CORE_TRACE("ini파일을 저장합니다.");
	}

	void Settings::Load()
	{
		if (!FileSystemHelper::FileExists(mFileName))
			Default();

		IniHelper ini(mFileName);

		// Window
		mWidth = ini["Window"]["Width"];
		mHeight = ini["Window"]["Height"];
		mbMaximize = ini["Window"]["bMaximize"];
		mbFullScreen = ini["Window"]["bFullScreen"];
		mbBorderless = ini["Window"]["bBorderless"];
	}

	void Settings::SetWidth(unsigned int width)
	{
		if ((width <= 0) || (mWidth == width))
		{
			CORE_ERROR("Settings::SetWidth >> 유효하지 않은 크기({:d})로 변경을 시도하였습니다.", width);
			return;
		}

		mWidth = width;
	}

	void Settings::SetHeight(unsigned int height)
	{
		if ((height <= 0) || (mHeight == height))
		{
			CORE_ERROR("Settings::SetHeight >> 유효하지 않은 크기({:d})로 변경을 시도하였습니다.", height);
			return;
		}

		mHeight = height;
	}
	eWindowModes Settings::GetWindowMode()
	{
		if (mbFullScreen)
		{
			mWindowMode = eWindowModes::FullScreen;
		}
		else if (mbBorderless)
		{
			mWindowMode = eWindowModes::Borderless;
		}
		else
		{
			mWindowMode = eWindowModes::Windowed;
		}

		return mWindowMode;
	}

	void Settings::SetWindowMode(eWindowModes mode)
	{
		mWindowMode = mode;

		switch (mWindowMode)
		{
		case eWindowModes::Windowed:
		{
			mbBorderless = false;
			mbFullScreen = false;
		}
		break;
		case eWindowModes::Borderless:
		{
			mbBorderless = true;
			mbFullScreen = false;
		}
		break;
		case eWindowModes::FullScreen:
		{
			mbBorderless = true;
			mbFullScreen = true;
		}
		break;
		default:
			return;
		}
	}
}