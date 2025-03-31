#pragma once
#include <string>
#include "imgui.h"
#include "dive.h"
#include "../Editor.h"

namespace Dive
{
	class EditorView
	{
	public:
		EditorView();
		virtual ~EditorView() = default;
		
		void Draw();

		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }

		bool IsVisible() const { return m_bVisible; }
		void SetVisible(bool visible) { m_bVisible = visible; }

		template <typename T, typename std::enable_if<std::is_base_of<EditorView, T>::value>::type* = nullptr>
		static std::shared_ptr<T> CreateView()
		{
			auto pView = std::make_shared<T>();
			m_Views.push_back(pView);
			return pView;
		}

		template <typename T, typename std::enable_if<std::is_base_of<EditorView, T>::value>::type* = nullptr>
		static std::shared_ptr<T> GetView()
		{
			auto it = std::find_if(m_Views.begin(), m_Views.end(), [](const std::shared_ptr<EditorView>& view)
				{
					return std::dynamic_pointer_cast<T>(view) != nullptr;
				}
			);
			if (it != m_Views.end())
			{
				return std::dynamic_pointer_cast<T>(*it);
			}
			else
			{
				return CreateView<T>();
			}
		}

		static void DrawViews();

	protected:
		virtual void drawView() {}

	protected:
		std::string m_Title;

		float m_Width;
		float m_Height;
		bool m_bVisible;

		static std::vector<std::shared_ptr<EditorView>> m_Views;
	};
}