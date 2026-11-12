#pragma once
#include "Scene.h"
#include <DirectXMath.h>
#include <stack>

namespace Sokoban
{
	struct StateRecord
	{
		DirectX::XMINT2 playerPos;
		std::vector<DirectX::XMINT2> cratePositions;
		int moveCount;
	};

	class PlayScene : public Scene
	{
	public:
		PlayScene(const GameContext& context);
		~PlayScene() override;

		void Initialize() override;
		void Cleanup() override;

		void Update(float deltaTime) override;

		void Render() override;
		void RenderUI() override;

	private:
	private:
		int m_moveCount = 0;
		std::stack<StateRecord> m_recordStack;
	};
}
