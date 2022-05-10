#pragma once
#include "Renderer.h"
#include "Events/EventSystem.h"
#include "Events/EngineEvents.h"

namespace Dive
{
	class Scene;
	class GameObject;
	class CommandList;

	class RenderPath
	{
	public:
		RenderPath();
		virtual ~RenderPath();

		virtual void Update(float delta);
		virtual void Render();

		void OnActiveScene(const Event& e);

	
	protected:
		void passDefault(CommandList* pCl);


	protected:
		Scene* m_pActiveScene = nullptr;

		Visibility m_MainVisibilities;
	};
}