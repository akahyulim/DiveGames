#pragma once
#include "Widget.h"

namespace editor
{
	class Inspector : public Widget
	{
	public:
		Inspector(Editor* pEditor);

		void TickVisible() override;

		// material�� ���� Resource�� ����ϱ⵵ �Ѵ�.
		// ����Ƽ�� ��� texture�� mesh������ �����ش�.
		static void SetInspectGameObject(dive::GameObject* pTarget);

	private:
		void showGameObject();
		void showTransform(dive::Transform* pTransform);
		void showCamera(dive::Camera* pCamera);
		void showMeshRenderer(dive::MeshRenderer* pMeshRenderer);
		void showLight(dive::Light* pLight);
		// ���� �ٸ� Components

		void showAddComponentButton();

	private:
		static dive::GameObject* m_pInspectedTarget;
	};
}