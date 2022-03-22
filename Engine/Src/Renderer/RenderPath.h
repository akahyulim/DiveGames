#pragma once
#include "Events/EventSystem.h"

namespace Dive
{
	class GameObject;
	class CommandList;

	enum class eRenderableType
	{
		Opaque,
		Transparent,
		Light
	};

	class RenderPath
	{
	public:
		RenderPath();
		virtual ~RenderPath();

		virtual void Update(float delta);
		virtual void Render();

		void OnAcquireRenderableObjects(const Event& e);

	private:
		void passDefault(CommandList* pCl);

	private:
		std::unordered_map<eRenderableType, std::vector<GameObject*>> m_RenderableObjects;
	};
}