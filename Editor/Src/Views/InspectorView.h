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

		void SetSelectedNode(Entity node) { m_SelectedNode = node; }

	private:
		void drawComponents(Entity entity);

		template<typename T>
		void displayAddComponentEntry(const std::string& entryName);

	private:
		Entity m_SelectedNode;
	};

	template<typename T>
	void InspectorView::displayAddComponentEntry(const std::string& entryName)
	{

	}
}
