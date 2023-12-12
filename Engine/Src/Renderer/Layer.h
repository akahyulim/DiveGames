#pragma once
#include "Scene/GameObject.h"
#include "Scene/Components/Camera.h"

namespace Dive
{
	// 관리는 Renderer
	class Layer
	{
	public:
		// 일단 렌더패쓰, 렉탱글 제외
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