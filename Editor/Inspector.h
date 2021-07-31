#pragma once
#include "Widget.h"

namespace editor
{
	class Inspector : public Widget
	{
	public:
		Inspector(Editor* pEditor);

		void TickVisible() override;

		// material과 같이 Resource를 출력하기도 한다.
		// 유니티의 경우 texture와 mesh까지도 보여준다.
		static void SetInspectGameObject(dive::GameObject* pTarget);

	private:
		void showGameObject();
		void showTransform(dive::Transform* pTransform);
		void showCamera(dive::Camera* pCamera);
		void showMeshRenderer(dive::MeshRenderer* pMeshRenderer);
		void showLight(dive::Light* pLight);
		// 이하 다른 Components

		void showAddComponentButton();

	private:
		static dive::GameObject* m_pInspectedTarget;
	};
}