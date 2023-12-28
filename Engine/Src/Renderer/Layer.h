#pragma once
#include "Scene/GameObject.h"
#include "Scene/Components/Camera.h"

namespace Dive
{
	class Layer
	{
	public:
		Layer();
		~Layer();

		void Update();
		void Render();

	private:
		void deferredRendering();

	private:
		Camera* m_pCamera = nullptr;
		std::unordered_map<eRendererGameObject, std::vector<GameObject*>> m_Renderables;
	};
}