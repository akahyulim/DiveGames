#pragma once
#include "Panel.h"

namespace Dive
{
	class InspectorPanel : public Panel
	{
	public:
		InspectorPanel(Editor* pEditor);

		void TickVisible() override;

		// material과 같이 Resource를 출력하기도 한다.
		// 유니티의 경우 texture와 mesh까지도 보여준다.
		static void SetInspectGameObject(Dive::GameObject* pTarget);

	private:
		void showGameObject();
		void showTransform(Dive::Transform* pTransform);
		void showCamera(Dive::Camera* pCamera);
		void showMeshRenderer(Dive::MeshRenderer* pMeshRenderer);
		void showLight(Dive::Light* pLight);
		// 이하 다른 Components

		void showAddComponentButton();

	private:
		static Dive::GameObject* m_pInspectedTarget;
	};
}