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

	// �׳� �� �����ڰ� �̱������� ������.
	// �ػ�, BackBuffer ũ��, ������ ũ��, �׸��� RenderTarget�� ũ��� �� �ٸ� �� �ִ�.
	// ���� Settings�� �ػ󵵿� BackBuffer ũ�� �׸��� ������ ũ�⸸ �ٷ�� �ȴ�.
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