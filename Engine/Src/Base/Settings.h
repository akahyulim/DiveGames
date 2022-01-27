#pragma once

namespace Dive
{
	enum class eWindowModes : unsigned short
	{
		Windowed,
		Borderless,
		FullScreen,
	};

	// 생각을 좀 해보니 클래스보단 구조체가 어울릴 것 같다.
	// 파일화는 Runtime에서 직접 해도 된다.
	// 그렇다면 WindowData와 GraphicsData 둘로 나누어서 관리할까나?
	class Settings
	{
	public:
		static Settings& GetInstance()
		{
			static Settings instance;
			return instance;
		}

		void Initialize(const std::string& title);

		void Save();
		void Load();

		uint32_t GetWidth() const { return m_Width; }
		void SetWidth(uint32_t width);

		uint32_t GetHeight() const { return m_Height; }
		void SetHeight(uint32_t height);

		bool IsMaximize() const { return m_bMaximize; }
		void SetMaximize(bool maximize) { m_bMaximize = maximize; }

		bool IsFullScreen() const { return m_bFullScreen; }
		void SetFullScreen(bool fullScreen) { m_bFullScreen = fullScreen; }

		bool IsBorderlessWindow() const { return m_bBorderless; }
		void SetBorderlessWindow(bool borderless) { m_bBorderless = borderless; }

		eWindowModes GetWindowMode();
		void SetWindowMode(eWindowModes mode);

	private:
		std::string m_FileName;

		// Window
		uint32_t m_Width			= 800;
		uint32_t m_Height			= 600;
		bool m_bMaximize			= false;
		bool m_bFullScreen			= false;
		bool m_bBorderless			= false;
		eWindowModes m_WindowMode	= eWindowModes::Windowed;
	};
}