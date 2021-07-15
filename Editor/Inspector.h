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
		// material�뵵 �־�� �ϳ�?

	private:
		void showTransform(dive::Transform* transform);
		// ���� �ٸ� Components

		void showAddComponentButton();

	private:
		static dive::GameObject* m_InspectedTarget;
	};
}