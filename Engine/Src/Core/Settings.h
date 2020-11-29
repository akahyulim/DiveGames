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
		std::string GetFileName()	const { return m_fileName; }
		eScreenMode GetScreenMode() const { return m_screenMode; }
		int GetResolutionWidth()	const { return m_resolutionWidth; }
		int GetResolutionHeight()	const { return m_resolutionHeight; }
		//unsigned int GetFrameRate() const { return m_FrameRate; }
		//void SetFrameRate(unsigned int rate) { m_FrameRate = rate; }
		bool IsVSync()				const { return m_bVSync; }

		void SetResolutionWidth(int width)		{ m_resolutionWidth = width; }
		void SetResolutionHeight(int height)	{ m_resolutionHeight = height; }
		void SetScrrenMode(eScreenMode mode)	{ m_screenMode = mode; }
		void SetVSync(bool vSync)				{ m_bVSync = vSync; }

	private:
		Settings();
		//~Settings() = default;


	private:
		std::string m_fileName;
		eScreenMode m_screenMode = eScreenMode::Windowed;
		int m_resolutionWidth = 0;
		int m_resolutionHeight = 0;
		//unsigned int m_FrameRate = 60;
		bool m_bVSync = false;
	};
}

