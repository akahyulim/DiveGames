#pragma once
#include "Scene.h"
#include "../Player.h"
#include "../Stage.h"
#include "../Graphics/MeshManager.h"
#include "Graphics/ConstantBuffer.h"
#include <memory>
#include <DirectXMath.h>
#include <vector>
#include <stack>
#include <array>

namespace Dive
{
	using TextureHandle = uint64_t;
	class RenderTexture;
}

namespace Sokoban
{
	enum class eSpriteId : uint8_t
	{
		Wall,
		Path,
		Goal,
		Crate,
		Player,
		None,
		Count
	};

	class PlayScene : public Scene
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

		eSpriteId TileTypeToSpriteId(uint8_t) const;
		Dive::TextureHandle GetTexture(eSpriteId id) const;

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

		std::array<Dive::TextureHandle, static_cast<size_t>(eSpriteId::Count)> m_sprites;

		Player m_player;
	};
}
