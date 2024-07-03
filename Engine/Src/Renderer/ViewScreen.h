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

		void passGBuffer();
		void passLight();
		void deferredRender();

	private:
		Graphics* m_pGraphics;
		Renderer* m_pRenderer;
		Scene* m_pActiveScene;
		Camera* m_pCamera;

		eRenderPathType m_RenderPath;

		std::unordered_map<eRenderableType, std::vector<GameObject*>> m_Renderables;

		// LightManager를 만드는 것도 생각해보자.
		GameObject* m_pDirectionalLight;
		std::vector<GameObject*> m_SpotLights;
		std::vector<GameObject*> m_PointLights;

		std::vector<GameObject*> m_OpaqueModels;
		std::vector<GameObject*> m_TransparentModels;

		GBuffer m_GBuffer;
		Frustum m_Frustum;
	};
}