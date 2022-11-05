#pragma once
#include "DiveEngine.h"

namespace Editor
{
	class Panel;
	
	class Editor : public Dive::Application
	{
	public:
		explicit Editor(Dive::Context* pContext);
		~Editor() override;

		void Setup() override;
		void Start() override;
		void Stop() override;

		void OnBeginRender(const Dive::Event& e);
		void OnEndRender(const Dive::Event& e);
		void OnWindowEvent(const Dive::Event& e);

		template<class T> T* GetPanel();

	private:
		void drawPanels();

	private:
		std::vector<std::unique_ptr<Panel>> m_Panels;
	};

	template<class T>
	T* Editor::GetPanel()
	{
		for (const auto& panel : m_Panels)
		{
			if (T* pPanel = dynamic_cast<T*>(panel.get()))
				return pPanel;
		}

		return nullptr;
	}
}