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
		Default,
		Normal,
		Bold,
		Large,
		Large_Bold,
		Max
	};

	class EditorContext
	{
	public:
		static EditorContext& GetInstance()
		{
			static EditorContext instance;
			return instance;
		}

		World* ActiveWorld = nullptr;
		
		entt::entity Selected = entt::null;
		entt::entity EditorCamera = entt::null;
		entt::entity MainCamera = entt::null;
	};

	class Editor
	{
	public:
		Editor();
		~Editor();

		void Run();

		template <typename T, typename std::enable_if<std::is_base_of<View, T>::value>::type* = nullptr>
		std::shared_ptr<T> GetWidget();

		ImFont* GetFont(Dive::eFontTypes type);

		void SetTitle(const std::wstring& text = L"");

	private:
		void loadResources();
		void beginUI();
		void endUI();

	private:
		std::array<ImFont*, static_cast<size_t>(Dive::eFontTypes::Max)> m_Fonts;

		std::shared_ptr<MenuBar> m_MenuBar;
		std::vector<std::shared_ptr<View>> m_Widgets;
	};

	template <typename T, typename std::enable_if<std::is_base_of<View, T>::value>::type*>
	std::shared_ptr<T> Editor::GetWidget()
	{
		for (const auto& widget : m_Widgets)
		{
			if (std::shared_ptr<T> pWidget = std::dynamic_pointer_cast<T>(widget))
			{
				return pWidget;
			}
		}

		return nullptr;
	}
}
