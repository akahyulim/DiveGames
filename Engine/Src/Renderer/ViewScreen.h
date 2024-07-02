#pragma once
#include "Renderer.h"
#include "Graphics/GBuffer.h"
#include "Math/Frustum.h"

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// enum class
	//-------------------------------------------------------------------------------------
	enum class eRenderableType
	{
		Opaque,
		Transparent,
		Light,
		Camera
	};

	//-------------------------------------------------------------------------------------
	// forward declarations
	//-------------------------------------------------------------------------------------
	class Graphics;
	class Renderer;
	class GameObject;
	class Scene;
	class Camera;
	class Renderable;
	class Light;
	class Mesh;
	class Material;

	class ViewScreen
	{
	public:
		ViewScreen(Camera* pCamera, eRenderPathType renderPath = eRenderPathType::Forward);
		~ViewScreen() = default;

		void Update();
		void Render();

	private:
		void passDepth();
		void passOpaqueDraw();
		void passTransparentDraw();

		void forwardRender();
		void deferredRender();

	private:
		Graphics* m_pGraphics;
		Renderer* m_pRenderer;
		Scene* m_pActiveScene;
		Camera* m_pCamera;

		eRenderPathType m_RenderPath;

		std::unordered_map<eRenderableType, std::vector<GameObject*>> m_Renderables;

		GBuffer m_GBuffer;
		Frustum m_Frustum;
	};
}