#pragma once

namespace Dive
{
	enum class eScreenMode
	{
		Windowed = 0,
		Borderless,
		FullScreen
	};

	enum class eTargetFrame
	{
	};

	enum class eVSync
	{
		On = 0,
		Off,
	};
	
	class Settings
	{
	public:
		static Settings& GetInstance()
		{
			static Settings instance;
			return instance;
		}

		void Initialize();

		//= Options =========================================================================
		eScreenMode GetScreenMode() const { return m_ScreenMode; }
		void SetScrrenMode(eScreenMode mode) { m_ScreenMode = mode; }
		unsigned int GetResolutionWidth() const { return m_ResolutionWidth; }
		void SetResolutionWidth(unsigned int width) { m_ResolutionWidth = width; }
		unsigned int GetResolutionHeight() const { return m_ResolutionHeight; }
		void SetResolutionHeight(unsigned int height) { m_ResolutionHeight = height; }
		unsigned int GetFrameRate() const { return m_FrameRate; }
		void SetFrameRate(unsigned int rate) { m_FrameRate = rate; }
		eVSync GetVSync() const { return m_VSync; }
		void SetVSync(eVSync vSync) { m_VSync = vSync; }

	private:
		Settings() = default;


	private:
		eScreenMode m_ScreenMode;
		unsigned int m_ResolutionWidth;
		unsigned int m_ResolutionHeight;
		unsigned int m_FrameRate;
		eVSync m_VSync;
	};
}

