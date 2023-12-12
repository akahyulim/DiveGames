#pragma once
#include "Scene/GameObject.h"
#include "Scene/Components/Camera.h"

namespace Dive
{
	// ������ Renderer
	class Layer
	{
	public:
		// �ϴ� �����о�, ���ʱ� ����
		Layer();
		~Layer();

		void Update();
		void Render();

	private:
		void deferredRender();

	private:
		std::unordered_map<eRendererGameObject, std::vector<GameObject*>> m_Renderables;
		Camera* m_pCamera;
	};
}