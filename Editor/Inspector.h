#pragma once
#include "Widget.h"

namespace DiveEditor
{
	class Inspector : public Widget
	{
	public:
		Inspector(Editor* pEditor);

		void TickVisible() override;

		// material과 같이 Resource를 출력하기도 한다.
		// 유니티의 경우 texture와 mesh까지도 보여준다.
		static void SetInspectGameObject(DiveEngine::GameObject* pTarget);

	private:
		void showGameObject();
		void showTransform(DiveEngine::Transform* pTransform);
		void showCamera(DiveEngine::Camera* pCamera);
		void showMeshRenderer(DiveEngine::MeshRenderer* pMeshRenderer);
		void showLight(DiveEngine::Light* pLight);
		// 이하 다른 Components

		void showAddComponentButton();

	private:
		static DiveEngine::GameObject* m_pInspectedTarget;
	};
}