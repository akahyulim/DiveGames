#pragma once
#include "Widget.h"

namespace DiveEditor
{
	class Inspector : public Widget
	{
	public:
		Inspector(Editor* pEditor);

		void TickVisible() override;

		// material�� ���� Resource�� ����ϱ⵵ �Ѵ�.
		// ����Ƽ�� ��� texture�� mesh������ �����ش�.
		static void SetInspectGameObject(DiveEngine::GameObject* pTarget);

	private:
		void showGameObject();
		void showTransform(DiveEngine::Transform* pTransform);
		void showCamera(DiveEngine::Camera* pCamera);
		void showMeshRenderer(DiveEngine::MeshRenderer* pMeshRenderer);
		void showLight(DiveEngine::Light* pLight);
		// ���� �ٸ� Components

		void showAddComponentButton();

	private:
		static DiveEngine::GameObject* m_pInspectedTarget;
	};
}