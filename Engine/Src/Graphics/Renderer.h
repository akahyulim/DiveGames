#pragma once
#include "Core/Object.h"
#include "Graphics.h"

namespace Dive
{
	class Context;
	class Graphics;
	class GameObject;
	class MeshFilter;
	class Renderable;
	class E_UPDATE_SCENE;
	class Command;

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
		void legacyShader();
		void renderEditor();

	private:
		std::weak_ptr<Graphics> m_graphics;
		ID3D11DeviceContext* m_deviceContext;

		bool m_bInitialized;

		std::unordered_map<eRenderableObjectType, std::vector<GameObject*>> m_gameObjects;

		MeshFilter* m_meshFilter;
		// ���� �ʱⰪ ������ ������ ���� �� �ִ�.
		// none���� �� ���� �߰��ؾ� �Ѵ�.
		eSamplerType m_samplerType;				// sampler�� �������� ���� �� �� �ִ� �� ����...
		eRasterizerState m_rasterizerState;
		eBlendState m_blendState;
		bool m_bDepthStencilEnabled;

		GameObject* m_selectedCamera = nullptr;

		// �ϴ� ���⿡...
		Command* m_command;
	};
}

