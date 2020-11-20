#pragma once

namespace Dive
{
	enum class eScreenMode : unsigned int
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

		void Initialize(const char* fileName = nullptr);
		virtual void Save();
		virtual bool Load();

		//= Options =========================================================================
		std::string GetFileName()	const { return m_FileName; }
		eScreenMode GetScreenMode() const { return m_ScreenMode; }
		int GetResolutionWidth()	const { return m_ResolutionWidth; }
		int GetResolutionHeight()	const { return m_ResolutionHeight; }
		//unsigned int GetFrameRate() const { return m_FrameRate; }
		//void SetFrameRate(unsigned int rate) { m_FrameRate = rate; }
		bool IsVSync()				const { return m_bVSync; }

		void SetResolutionWidth(int width)		{ m_ResolutionWidth = width; }
		void SetResolutionHeight(int height)	{ m_ResolutionHeight = height; }
		void SetScrrenMode(eScreenMode mode)	{ m_ScreenMode = mode; }
		void SetVSync(bool vSync)				{ m_bVSync = vSync; }

	private:
		Settings();
		//~Settings() = default;


	private:
		std::string m_FileName;
		eScreenMode m_ScreenMode = eScreenMode::Windowed;
		int m_ResolutionWidth = 0;
		int m_ResolutionHeight = 0;
		//unsigned int m_FrameRate = 60;
		bool m_bVSync = false;
	};
}

