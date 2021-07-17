#pragma once
#include "Widget.h"

namespace editor
{
	class Inspector : public Widget
	{
	public:
		Inspector(Editor* editor);

		void TickVisible() override;

		// material�� ���� Resource�� ����ϱ⵵ �Ѵ�.
		// ����Ƽ�� ��� texture�� mesh������ �����ش�.
		static void SetInspectGameObject(dive::GameObject* target);

	private:
		void showGameObject();
		void showTransform(dive::Transform* transform);
		void showCamera(dive::Camera* camera);
		void showMeshRenderer(dive::MeshRenderer* meshRenderer);
		// ���� �ٸ� Components

		void showAddComponentButton();

	private:
		static dive::GameObject* m_InspectedTarget;
	};
}