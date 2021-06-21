#pragma once
#include <string>

namespace Dive
{
	enum class eWindowModes : unsigned short
	{
		Windowed,
		Borderless,
		FullScreen,
	};

	// 그냥 다 때려박고 싱글톤으로 만들자.
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

		unsigned int GetWidth() const { return m_width; }
		void SetWidth(unsigned int width);

		unsigned int GetHeight() const { return m_height; }
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
		std::string m_filename;

		// window
		unsigned int m_width;
		unsigned int m_height;
		bool m_bMaximize;
		bool m_bFullScreen;
		bool m_bBorderless;
		eWindowModes m_windowMode;
	};
}