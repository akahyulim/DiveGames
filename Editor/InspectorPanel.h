#pragma once
#include "Panel.h"

namespace Dive
{
	class InspectorPanel : public Panel
	{
	public:
		InspectorPanel(Editor* pEditor);

		void TickVisible() override;

		// material�� ���� Resource�� ����ϱ⵵ �Ѵ�.
		// ����Ƽ�� ��� texture�� mesh������ �����ش�.
		static void SetInspectGameObject(Dive::GameObject* pTarget);

	private:
		void showGameObject();
		void showTransform(Dive::Transform* pTransform);
		void showCamera(Dive::Camera* pCamera);
		void showMeshRenderer(Dive::MeshRenderer* pMeshRenderer);
		void showLight(Dive::Light* pLight);
		// ���� �ٸ� Components

		void showAddComponentButton();

	private:
		static Dive::GameObject* m_pInspectedTarget;
	};
}