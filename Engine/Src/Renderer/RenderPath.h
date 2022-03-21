#pragma once

namespace Dive
{
	class GameObject;
	class CommandList;

	enum class eRenderObjectType
	{
		Opaque,
		Transparent,
		Light
	};

	class RenderPath
	{
	public:
		virtual ~RenderPath() = default;

		virtual void Update(float delta);
		virtual void Render();

		void OnAcuireRenderObjects(std::vector<GameObject*>& gameObjects);

	private:
		void passDefault(CommandList* pCl);

	private:
		std::unordered_map<eRenderObjectType, std::vector<GameObject*>> m_RenderObjects;
	};
}