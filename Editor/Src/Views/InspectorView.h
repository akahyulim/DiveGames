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

	private:
		void drawComponents(entt::entity entity);

		template<typename T>
		void displayAddComponentEntry(const std::string& entryName);

	private:
	};

	template<typename T>
	void InspectorView::displayAddComponentEntry(const std::string& entryName)
	{

	}
}
