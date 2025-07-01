#pragma once
#include <dive.h>
#include <imgui.h>

#include "Views/View.h"

namespace Dive 
{
	class Project;
	class MenuBar;

	enum class eFontTypes
	{
		Normal,
		Bold,
		Large,
		Large_Bold,
		Max
	};
	
	class EditorContext
	{
	public:
		static World* ActiveWorld;
		
		static GameObject* Selected;
		static GameObject* EditorCamera;
		static GameObject* MainCamera;
	};

	class Editor
	{
	public:
		Editor();
		~Editor();

		void Run();

		ImFont* GetFont(Dive::eFontTypes type);

		void SetTitle(const std::wstring& text = L"");

	private:
		void loadResources();
		void beginUI();
		void endUI();

	private:
		std::array<ImFont*, static_cast<size_t>(Dive::eFontTypes::Max)> m_Fonts;

		std::unique_ptr<MenuBar> m_MenuBar;
		std::vector<std::unique_ptr<View>> m_Widgets;
	};
}
