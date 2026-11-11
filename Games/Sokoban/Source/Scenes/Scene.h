#pragma once
#include "../Game.h"

namespace Sokoban
{
	class Scene
	{
	public:
		Scene(const GameContext& context)
			: m_gameContext(context){ }
		virtual ~Scene() = default;

		virtual void Initialize() = 0;
		virtual void Cleanup() = 0;

		virtual void Update(float deltaTime) = 0;
		
		virtual void Render() = 0;
		virtual void RenderUI() {}

	protected:
		GameContext m_gameContext;
	};
}