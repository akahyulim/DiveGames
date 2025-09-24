#pragma once
#include <dive.h>
#include "views/view.h"

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
		static void Clear()
		{
			ActiveWorld = nullptr;
			Selected = nullptr;
			EditorCamera = nullptr;
			MainCamera = nullptr;
		}

		static World* ActiveWorld;
		static GameObject* Selected;
		static GameObject* EditorCamera;
		static GameObject* MainCamera;
	};

	class Editor : public Application
	{
	public:
		Editor();
		~Editor() override = default;

		void Setup() override;
		void Start() override;
		void Stop() override;

		void OnPostRender();

		void SetTitle(const std::wstring& worldName = std::wstring());

	private:
		std::array<ImFont*, static_cast<size_t>(Dive::eFontTypes::Max)> m_Fonts;

		std::unique_ptr<MenuBar> m_MenuBar;
		std::vector<std::unique_ptr<View>> m_Views;
	};
}