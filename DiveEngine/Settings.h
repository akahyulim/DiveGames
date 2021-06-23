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

		unsigned int GetWidth() const { return mWidth; }
		void SetWidth(unsigned int width);

		unsigned int GetHeight() const { return mHeight; }
		void SetHeight(unsigned int height);

		bool IsMaximize() const { return mbMaximize; }
		void SetMaximize(bool maximize) { mbMaximize = maximize; }

		bool IsFullScreen() const { return mbFullScreen; }
		void SetFullScreen(bool fullScreen) { mbFullScreen = fullScreen; }

		bool IsBorderlessWindow() const { return mbBorderless; }
		void SetBorderlessWindow(bool borderless) { mbBorderless = borderless; }

		eWindowModes GetWindowMode();
		void SetWindowMode(eWindowModes mode);


	private:
		std::string mFileName;

		// window
		unsigned int mWidth;
		unsigned int mHeight;
		bool mbMaximize;
		bool mbFullScreen;
		bool mbBorderless;
		eWindowModes mWindowMode;
	};
}