#pragma once
#include "Core/Object.h"
#include "Graphics.h"

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

	private:
		//= d3d11 pipeline???
		// IASetInputLayout
		// IASetVerteBuffer
		// IASetIndexBuffer
		void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology);
		// VSSetShader
		// VSSetConstantBuffers
		// PSSetShader
		// PSSetConstantBuffers
		void setSampler(eSamplerType type);
		void setRasterizerState(eRasterizerState state);
		// OMSetRenderTargets
		void setBlendState(eBlendState state);
		void setDepthStencilState(bool enabled);

		void testRender();

	private:
		std::weak_ptr<Graphics> m_graphics;
		ID3D11DeviceContext* m_deviceContext;

		bool m_bInitialized;

		std::unordered_map<eRenderableObjectType, std::vector<GameObject*>> m_gameObjects;

		// 전부 초기값 때문에 문제가 생길 수 있다.
		// none같은 걸 전부 추가해야 한다.
		eSamplerType m_samplerType;				// sampler는 여러개를 전달 할 수 있는 것 같다...
		eRasterizerState m_rasterizerState;
		eBlendState m_blendState;
		bool m_bDepthStencilEnabled;
	};
}

