#pragma once

namespace Dive
{
	enum class eScreenMode
	{
		Windowed = 0,
		Borderless,
		FullScreen
	};

	enum class eFrameRate
	{
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
		virtual bool Save();
		virtual bool Load();

		//= Options =========================================================================
		eScreenMode GetScreenMode() const { return m_ScreenMode; }
		void SetScrrenMode(eScreenMode mode) { m_ScreenMode = mode; }
		unsigned int GetResolutionWidth() const { return m_ResolutionWidth; }
		void SetResolutionWidth(unsigned int width) { m_ResolutionWidth = width; }
		unsigned int GetResolutionHeight() const { return m_ResolutionHeight; }
		void SetResolutionHeight(unsigned int height) { m_ResolutionHeight = height; }
		//unsigned int GetFrameRate() const { return m_FrameRate; }
		//void SetFrameRate(unsigned int rate) { m_FrameRate = rate; }
		bool IsVSync() const { return m_bVSync; }
		void SetVSync(bool vSync) { m_bVSync = vSync; }

	private:
		Settings() = default;


	private:
		eScreenMode m_ScreenMode = eScreenMode::Windowed;
		unsigned int m_ResolutionWidth = 0;
		unsigned int m_ResolutionHeight = 0;
		//unsigned int m_FrameRate = 60;
		bool m_bVSync = false;
	};
}

