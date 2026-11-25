#pragma once
#include "Scene.h"
#include "../Stage.h"
#include "../Graphics/MeshManager.h"
#include "Graphics/ConstantBuffer.h"
#include <memory>
#include <DirectXMath.h>
#include <vector>
#include <stack>

namespace Dive
{
	class RenderTexture;
}

namespace Sokoban
{
	class PlayScene2D : public Scene
	{
		struct FrameData
		{
			DirectX::XMMATRIX viewProj;
		};

		struct ObjectData
		{
			DirectX::XMMATRIX model;
		};

		struct StateRecord
		{
			DirectX::XMINT2 playerPos;
			std::vector<DirectX::XMINT2> cratePositions;
			int moveCount;
		};

	public:
		PlayScene2D(const GameContext& context);
		~PlayScene2D() override;

		void Initialize() override;
		void Cleanup() override;

		void Update(float deltaTime) override;

		void Render() override;
		void RenderUI() override;

		void LoadStage(int level);
		void LoadStage(const std::string& filepath);

		int FindCrateAt(int x, int y);

	private:
		void checkAndMovePlayer(int nextX, int nextY, int dx, int dy);
		bool checkGameClear();

		void undo();

		void showDialogBox();

	private:
		std::vector<Stage> m_stages;
		int m_stageLevel = 0;
		Stage m_currentStage;

		int m_moveCount = 0;
		std::stack<StateRecord> m_recordStack;

		bool m_showDialog = false;
	};
}
