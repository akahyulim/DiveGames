#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Graphics;
	class GameObject;
	class E_UPDATE_SCENE;

	enum class eRenderableObjectType
	{
		Opaque,
		Transparent,
		Light,
		Camera,
	};

	class Renderer : public Object
	{
		DIVE_OBJECT(Renderer, Object);

	public:
		Renderer(Context* context);
		~Renderer();

		void Initialize();
		void Render();

		bool IsInitialized() const { return m_bInitialized; }

		void OnAcquireRenderable(const E_UPDATE_SCENE* evnt);

		//= d3d11 pipeline???
		// IASetInputLayout
		// IASetVerteBuffer
		// IASetIndexBuffer
		// IASetPrimitiveTopology
		// VSSetShader
		// VSSetConstantBuffers
		// PSSetShader
		// PSSetConstantBuffers
		// PSSetSamplers
		// RSSetState
		// OMSetRenderTargets
		// OMSetBlendState
		// OMSetDepthStencilState

	private:
		void testRender();

	private:
		std::weak_ptr<Graphics> m_graphics;

		bool m_bInitialized;

		std::unordered_map<eRenderableObjectType, std::vector<GameObject*>> m_gameObjects;
	};
}

