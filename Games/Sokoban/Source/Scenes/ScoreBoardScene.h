#pragma once
#include "Scene.h"

namespace Sokoban
{
	class ScoreBoardScene : public Scene
	{
	public:
		ScoreBoardScene(const GameContext& context);
		~ScoreBoardScene() override;

		void Initialize() override;
		void Cleanup() override;

		void Update(float deltaTime) override;

		void Render() override;
		void RenderUI() override;
	};
}