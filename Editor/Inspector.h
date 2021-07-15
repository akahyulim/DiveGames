#pragma once
#include "Widget.h"

namespace editor
{
	class Inspector : public Widget
	{
	public:
		Inspector(Editor* editor);

		void TickVisible() override;

		static void SetInspectGameObject(dive::GameObject* target);
		// material용도 있어야 하나?

	private:
		void showTransform(dive::Transform* transform);
		// 이하 다른 Components

		void showAddComponentButton();

	private:
		static dive::GameObject* m_InspectedTarget;
	};
}