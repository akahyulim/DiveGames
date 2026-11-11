#pragma once
#include "Scene.h"

namespace Sokoban
{
	class TitleScene : public Scene
	{
	public:
		TitleScene(const GameContext& context);
		~TitleScene() override;

		void Initialize() override;
		void Cleanup() override;

		void Update(float deltaTime) override;

		void Render() override;
		void RenderUI() override;
	};
}