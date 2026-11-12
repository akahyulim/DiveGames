#pragma once
#include "Scene.h"

namespace Sokoban
{
	class EditorScene : public Scene
	{
	public:
		EditorScene(const GameContext& context);
		~EditorScene() override;

		void Initialize() override;
		void Cleanup() override;

		void Update(float deltaTime) override;

		void Render() override;
		void RenderUI() override;
	};
}