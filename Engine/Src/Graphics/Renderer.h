#pragma once
#include "Core/Object.h"
#include "Graphics.h"
#include "GraphicsEnums.h"

namespace Dive
{
	class Context;
	class Graphics;
	class GameObject;
	class MeshFilter;
	class MeshRenderer;
	class E_UPDATE_SCENE;
	class Command;

	enum class eRenderableObjectType
	{
		Camera,
		Light,
		Opaque,
		Transparent,
		Skybox,
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
		// pass
		void pass_Color();

	private:
		Graphics* m_graphics;
		ID3D11DeviceContext* m_deviceContext;

		bool m_bInitialized;

		std::unordered_map<eRenderableObjectType, std::vector<GameObject*>> m_gameObjects;
		GameObject* m_selectedCamera;

		// 일단 여기에...
		std::unique_ptr<Command> m_command;
	};
}

