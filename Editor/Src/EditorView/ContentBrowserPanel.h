#pragma once
#include <filesystem>

namespace Dive
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnRender();

	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
	};
}
