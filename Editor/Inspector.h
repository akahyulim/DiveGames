#pragma once
#include "Widget.h"

namespace editor
{
	class Inspector : public Widget
	{
	public:
		Inspector(Editor* editor);

		void TickVisible() override;

		// material과 같이 Resource를 출력하기도 한다.
		// 유니티의 경우 texture와 mesh까지도 보여준다.
		static void SetInspectGameObject(dive::GameObject* target);

	private:
		void showGameObject();
		void showTransform(dive::Transform* transform);
		void showCamera(dive::Camera* camera);
		void showMeshRenderer(dive::MeshRenderer* meshRenderer);
		// 이하 다른 Components

		void showAddComponentButton();

	private:
		static dive::GameObject* m_InspectedTarget;
	};
}