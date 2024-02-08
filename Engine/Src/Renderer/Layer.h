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

		inline bool IsVisible() const { return m_bVisible; }
		inline void SetVisible(bool visible) { m_bVisible = visible; }

	private:
		void deferredRendering();

	private:
		bool m_bVisible;

		Camera* m_pCamera;
		std::unordered_map<eRendererGameObject, std::vector<GameObject*>> m_Renderables;
	};
}