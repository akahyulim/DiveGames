#pragma once
#include <string>

namespace dive
{
	enum class eWindowModes : unsigned short
	{
		Windowed,
		Borderless,
		FullScreen,
	};

	// 그냥 다 때려박고 싱글톤으로 만들자.
	// 해상도, BackBuffer 크기, 윈도우 크기, 그리고 RenderTarget의 크기는 다 다를 수 있다.
	// 물론 Settings는 해상도와 BackBuffer 크기 그리고 윈도우 크기만 다루면 된다.
	class Settings
	{
	public:
		static Settings& GetInstance()
		{
			static Settings instance;
			return instance;
		}

		void Initialize(const std::string& title);

		void Default();

		void Save();
		void Load();

		unsigned int GetWidth() const { return m_Width; }
		void SetWidth(unsigned int width);

		unsigned int GetHeight() const { return m_Height; }
		void SetHeight(unsigned int height);

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

		// window
		unsigned int m_Width;
		unsigned int m_Height;
		bool m_bMaximize;
		bool m_bFullScreen;
		bool m_bBorderless;
		eWindowModes m_WindowMode;
	};
}