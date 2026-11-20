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

		MeshManager m_blockManager;

		int m_moveCount = 0;
		std::stack<StateRecord> m_recordStack;

		std::unique_ptr<Dive::ConstantBuffer<FrameData>> m_cbFrame;
		std::unique_ptr<Dive::ConstantBuffer<ObjectData>> m_cbObject;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_dss;
		std::unique_ptr<Dive::RenderTexture> m_rt;
		std::unique_ptr<Dive::RenderTexture> m_dsv;
		Dive::RenderPassDesc m_offScreenResolvePass;

		bool m_showDialog = false;
	};
}
