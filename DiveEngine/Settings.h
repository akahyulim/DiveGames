#pragma once
#include <string>

namespace Dive
{
	// �׳� �� �����ڰ� �̱������� ������.
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

		bool IsFullScreen() const { return m_bFullScreen; }

		bool IsBorderlessWindow() const { return m_bBorderless; }

	private:

	private:
		std::string m_filename;

		// window
		unsigned int m_width;
		unsigned int m_height;
		bool m_bMaximize;
		bool m_bFullScreen;
		bool m_bBorderless;
	};
}