#pragma once
#include "View.h"

namespace Dive
{
	class InspectorView : public View
	{
	public:
		InspectorView(Editor* editor);
		virtual ~InspectorView();

		void drawView() override;

		void SetSelectedNode(entt::entity node) { m_SelectedNode = node; }

	private:
		void drawComponents(entt::entity entity);

		template<typename T>
		void displayAddComponentEntry(const std::string& entryName);

	private:
		entt::entity m_SelectedNode = entt::null;
	};

	template<typename T>
	void InspectorView::displayAddComponentEntry(const std::string& entryName)
	{

	}
}
