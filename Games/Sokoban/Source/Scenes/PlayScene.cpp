#include "PlayScene.h"
#include "../Graphics/Camera.h"
#include "Core/Window.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/RenderTexture.h"
#include "Input/Input.h"
#include "Core/LogManager.h"
#include "Rendering/Renderer2D.h"
#include <imgui.h>

namespace Sokoban
{
	PlayScene::PlayScene(const GameContext& context)
		: Scene(context)
	{
	}

	PlayScene::~PlayScene()
	{
	}

	void PlayScene::Initialize()
	{
		{
			auto camera = m_gameContext.camera;// ->LookAt({ 0.0f, 0.0f, -30.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
			// 예시: (x, y) 위치에 있는 2D 카메라
			camera->LookAt(
				{ 0.0f,	0.0f, -10.0f }, // eye
				{ 0.0f, 0.0f, 0.0f }, // target
				{ 0.0f, 1.0f, 0.0f }  // up (+Y가 위쪽)
			);
			camera->SetOrthographic(1600, 900, 0.1f, 100.0f);
			constexpr float aspectRatio = 1280.0f / 720.0f;
			constexpr float orthoHeight = 16.0f;//12.0f;
			constexpr float orthoWidth = orthoHeight * aspectRatio; // 약 21.333f

			//camera->SetOrthographic(orthoWidth, orthoHeight, 0.1f, 100.0f);
		}

		{
			const std::filesystem::path dir = "Assets/Textures/kenney_sokoban-pack/PNG/Default size";
			m_sprites[static_cast<size_t>(eSpriteId::Wall)] = Dive::TextureManager::Get().LoadTexture(dir / "Blocks/block_03.png");
			m_sprites[static_cast<size_t>(eSpriteId::Path)] = Dive::TextureManager::Get().LoadTexture(dir / "Ground/ground_06.png"); 
			m_sprites[static_cast<size_t>(eSpriteId::Goal)] = Dive::TextureManager::Get().LoadTexture(dir / "Ground/ground_03.png");
			m_sprites[static_cast<size_t>(eSpriteId::Crate)] = Dive::TextureManager::Get().LoadTexture(dir / "Crates/crate_03.png");
			m_sprites[static_cast<size_t>(eSpriteId::Player)] = Dive::TextureManager::Get().LoadTexture(dir / "Player/player_01.png");
			m_sprites[static_cast<size_t>(eSpriteId::None)] = Dive::INVALID_TEXTURE_HANDLE;

			m_player.SetFrameHandle(ePlayerDir::Down, 0, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_05.png"));
			m_player.SetFrameHandle(ePlayerDir::Down, 1, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_06.png"));
			m_player.SetFrameHandle(ePlayerDir::Down, 2, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_07.png"));

			m_player.SetFrameHandle(ePlayerDir::Up, 0, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_08.png"));
			m_player.SetFrameHandle(ePlayerDir::Up, 1, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_09.png"));
			m_player.SetFrameHandle(ePlayerDir::Up, 2, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_10.png"));

			m_player.SetFrameHandle(ePlayerDir::Left, 0, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_20.png"));
			m_player.SetFrameHandle(ePlayerDir::Left, 1, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_21.png"));
			m_player.SetFrameHandle(ePlayerDir::Left, 2, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_22.png"));

			m_player.SetFrameHandle(ePlayerDir::Right, 0, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_17.png"));
			m_player.SetFrameHandle(ePlayerDir::Right, 1, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_18.png"));
			m_player.SetFrameHandle(ePlayerDir::Right, 2, Dive::TextureManager::Get().LoadTexture(dir / "Player/player_19.png"));
		}

		{
			m_stages.push_back(LevelOne());
			m_stages.push_back(LevelTwo());
			m_stages.push_back(LevelThree());

			LoadStage(0);
		}
	}

	void PlayScene::Cleanup()
	{
		m_stages.clear();
	}

	void PlayScene::Update(float deltaTime)
	{
		if (checkGameClear())
		{
			// 대화상자가 뜨고 Next 버튼을 누르면 넘어가도록 하자.
			// 축하 메시지와 함께 점수를 보여줄 수도 있다.
			LoadStage(++m_stageLevel);
		}

		auto input = m_gameContext.input;

		m_player.Update(deltaTime);

		{
			if (input->KeyDown(DIK_ESCAPE))
			{
				m_showDialog = !m_showDialog;
			}

			if (input->KeyDown(DIK_R))
			{
				undo();
				return;
			}

			if (input->KeyDown(DIK_G))
			{
				// 아직은 이르다. 
				// 스왑이 가능하려면 Shader, Constant Buffer 등을 정리해야 한다.
				//m_gameContext.changeScene(eSceneType::Play3D);
				return;
			}

			int dx = 0;
			int dy = 0;

			if (input->KeyDown(DIK_LEFT) || input->KeyDown(DIK_A))
			{
				dx = -1;
			}
			else if (input->KeyDown(DIK_RIGHT) || input->KeyDown(DIK_D))
			{
				dx = 1;
			}
			else if (input->KeyDown(DIK_UP) || input->KeyDown(DIK_W))
			{
				dy = -1;
			}
			else if (input->KeyDown(DIK_DOWN) || input->KeyDown(DIK_S))
			{
				dy = 1;
			}

			if (dx != 0 || dy != 0)
			{
				int nextX = m_currentStage.playerPos.x + dx;
				int nextY = m_currentStage.playerPos.y + dy;

				checkAndMovePlayer(nextX, nextY, dx, dy);
			}
		}
	}

	void PlayScene::Render()
	{
		if (m_gameContext.renderer == nullptr || m_gameContext.camera == nullptr)
			return;

		m_gameContext.renderer->BeginScene(
			DirectX::XMMatrixTranspose(
				m_gameContext.camera->GetViewMatrix() *
				m_gameContext.camera->GetProjectionMatrix()));

		{
			// 카메라가 1280x720 픽셀 단위 투영을 쓰면 tileSize를 64.0f 정도로,
			// 카메라 ortho 범위를 ~20x11 월드 단위로 줄였다면 tileSize를 1.0f로 두면 됩니다.
			constexpr float tileSize = 64.0f;

			const float halfWidth = static_cast<float>(m_currentStage.size.x) * 0.5f;
			const float halfHeight = static_cast<float>(m_currentStage.size.y) * 0.5f;

			// 스테이지 전체가 (0, 0) 중심에 오도록 2D(XY) 평면 좌표 계산
			auto GetTileWorldPos = [halfWidth, halfHeight, tileSize](int x, int invertedY, float zOffset)
				{
					const float worldX = (static_cast<float>(x) - halfWidth + 0.5f) * tileSize;
					const float worldY = (static_cast<float>(invertedY) - halfHeight + 0.5f) * tileSize;
					return DirectX::XMFLOAT3{ worldX, worldY, zOffset };
				};

			// 보간된 실수 격자(gridX, invertedGridY) 기준 월드 좌표 계산
			auto GetInterpolatedWorldPos = [halfWidth, halfHeight, tileSize](float gridX, float invertedGridY, float zOffset)
				{
					const float worldX = (gridX - halfWidth + 0.5f) * tileSize;
					const float worldY = (invertedGridY - halfHeight + 0.5f) * tileSize;
					return DirectX::XMFLOAT3{ worldX, worldY, zOffset };
				};

			// Floor, Goal, Wall 그리기
			for (int y = 0; y < m_currentStage.size.y; ++y)
			{
				for (int x = 0; x < m_currentStage.size.x; ++x)
				{
					const uint8_t type = m_currentStage.gridTiles[y * m_currentStage.size.x + x];
					const Dive::TextureHandle sprite = GetTexture(TileTypeToSpriteId(type));
					if (sprite == Dive::INVALID_TEXTURE_HANDLE)
						continue;

					const int invertedY = m_currentStage.size.y - 1 - y;
					const DirectX::XMFLOAT3 pos = GetTileWorldPos(x, invertedY, 0.0f);

					m_gameContext.renderer->DrawQuad(pos, {tileSize, tileSize}, sprite);
				}
			}

			// 플레이어와 박스도 같은 좌표 규칙으로 위에 얹기
			/*
			if (m_sprites[static_cast<size_t>(eSpriteId::Player)] != Dive::INVALID_TEXTURE_HANDLE)
			{
				const int playerInvY = m_currentStage.size.y - 1 - m_currentStage.playerPos.y;
				const DirectX::XMFLOAT3 playerPos = GetTileWorldPos(m_currentStage.playerPos.x, playerInvY, -0.1f);
				m_gameContext.renderer->DrawQuad(playerPos, { tileSize, tileSize }, m_sprites[static_cast<size_t>(eSpriteId::Player)]);
			}
			*/

			// 플레이어(보간 좌표 + 현재 3프레임 핸들) 그리기
			const Dive::TextureHandle playerSprite = m_player.GetCurrentHandle();
			if (playerSprite != Dive::INVALID_TEXTURE_HANDLE)
			{
				const DirectX::XMFLOAT2 renderGridPos = m_player.GetRenderGridPos();
				const float playerInvY = static_cast<float>(m_currentStage.size.y - 1) - m_currentStage.playerPos.y; //renderGridPos.y;
				//const DirectX::XMFLOAT3 playerWorldPos = GetInterpolatedWorldPos(renderGridPos.x, playerInvY, -0.1f);
				const DirectX::XMFLOAT3 playerPos = GetTileWorldPos(m_currentStage.playerPos.x, (int)playerInvY, -0.1f);

				m_gameContext.renderer->DrawQuad(playerPos, { tileSize, tileSize }, playerSprite);
			}

			if (m_sprites[static_cast<size_t>(eSpriteId::Crate)] != Dive::INVALID_TEXTURE_HANDLE)
			{
				for (const auto& cratePos : m_currentStage.cratePosList)
				{
					const int crateInvY = m_currentStage.size.y - 1 - cratePos.y;
					const DirectX::XMFLOAT3 pos = GetTileWorldPos(cratePos.x, crateInvY, -0.1f);
					m_gameContext.renderer->DrawQuad(pos, { tileSize, tileSize }, m_sprites[static_cast<size_t>(eSpriteId::Crate)]);
				}
			}
		}

		m_gameContext.renderer->EndScene();
	}

	void PlayScene::RenderUI()
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		static ImVec2 lastWorkSize = ImVec2(0, 0);
		static ImVec2 lastWorkPos = ImVec2(0, 0);

		if (lastWorkSize.x != viewport->WorkSize.x || lastWorkSize.y != viewport->WorkSize.y ||
			lastWorkPos.x != viewport->WorkPos.x || lastWorkPos.y != viewport->WorkPos.y)
		{
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);

			lastWorkSize = viewport->WorkSize;
			lastWorkPos = viewport->WorkPos;
		}

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus;

		if (ImGui::Begin("SceneView", nullptr, windowFlags))
		{
			ImVec2 viewportPos = ImGui::GetCursorScreenPos();
			ImVec2 viewportSize = ImGui::GetContentRegionAvail();

			auto srv = m_gameContext.renderer->GetShaderResourceView();
			ImGui::Image((ImTextureID)srv, viewportSize);
		}

		ImGui::End();

		showDialogBox();
	}

	void PlayScene::LoadStage(int level)
	{
		if (level >= m_stages.size())
		{
			// POP UP 
			// - Congretuation
			// - next : score board
			// - exit
			//DV_LOG(Sokoban, info, "All stages cleared!");
			return;
		}

		m_stageLevel = level;
		m_currentStage = m_stages[level];

		std::stack<StateRecord>().swap(m_recordStack);

		StateRecord record;
		record.playerPos = m_currentStage.playerPos;
		record.cratePositions = m_currentStage.cratePosList;
		record.moveCount = m_moveCount = 0;
		m_recordStack.push(record);

		m_player.SnapToGrid(m_currentStage.playerPos);
	}

	void PlayScene::LoadStage(const std::string& filepath)
	{
	}

	int PlayScene::FindCrateAt(int x, int y)
	{
		for (int i = 0; i < m_currentStage.cratePosList.size(); ++i)
		{
			if (m_currentStage.cratePosList[i].x == x && m_currentStage.cratePosList[i].y == y)
			{
				return static_cast<int>(i);
			}
		}

		return -1;
	}

	eSpriteId PlayScene::TileTypeToSpriteId(uint8_t tileType) const
	{
		switch (tileType)
		{
		case TileType::Wall:	return eSpriteId::Wall;
		case TileType::Floor:	return eSpriteId::Path;
		case TileType::Goal:	return eSpriteId::Goal;
		case TileType::Empty:
		default:				return eSpriteId::None;
		}
	}

	Dive::TextureHandle PlayScene::GetTexture(eSpriteId id) const
	{
		return m_sprites[static_cast<size_t>(id)];
	}

	void PlayScene::checkAndMovePlayer(int nextX, int nextY, int dx, int dy)
	{
		// 맵 밖으로 이동 불가
		if (nextX < 0 || nextX >= m_currentStage.size.x || nextY < 0 || nextY >= m_currentStage.size.y)
		{
			return;
		}

		uint8_t nextTileType = m_currentStage.gridTiles[nextY * m_currentStage.size.x + nextX];

		// 벽 이동 불가
		if (nextTileType == TileType::Empty || nextTileType == TileType::Wall)
		{
			return;
		}

		int crateIndex = FindCrateAt(nextX, nextY);
		if (crateIndex != -1)
		{
			int nextCrateX = nextX + dx;
			int nextCrateY = nextY + dy;

			// 상자의 다음칸이 맵 밖이면 이동 불가
			if (nextCrateX < 0 || nextCrateX >= m_currentStage.size.x ||
				nextCrateY < 0 || nextCrateY >= m_currentStage.size.y)
			{
				return;
			}

			// 상자의 다음칸이 벽이거나 다른 상자면 이동 불가  
			uint8_t crateDestinationType = m_currentStage.gridTiles[nextCrateY * m_currentStage.size.x + nextCrateX];
			if (crateDestinationType == TileType::Empty || crateDestinationType == TileType::Wall || FindCrateAt(nextCrateX, nextCrateY) != -1)
			{
				return;
			}

			m_currentStage.cratePosList[crateIndex].x = nextCrateX;
			m_currentStage.cratePosList[crateIndex].y = nextCrateY;
		}

		const DirectX::XMINT2 prevPos = m_currentStage.playerPos;

		m_currentStage.playerPos.x = nextX;
		m_currentStage.playerPos.y = nextY;
		m_moveCount++;

		// 이동 결과 저장
		StateRecord record;
		record.playerPos = m_currentStage.playerPos;
		record.cratePositions = m_currentStage.cratePosList;
		record.moveCount = m_moveCount;
		m_recordStack.push(record);

		ePlayerDir dir = m_player.GetDirection();
		if (dx < 0)      dir = ePlayerDir::Left;
		else if (dx > 0) dir = ePlayerDir::Right;
		else if (dy < 0) dir = ePlayerDir::Up;
		else if (dy > 0) dir = ePlayerDir::Down;

		m_player.StartMove(prevPos, m_currentStage.playerPos, dir);
	}

	bool PlayScene::checkGameClear()
	{
		if (m_currentStage.cratePosList.empty())
			return false;

		for (const auto& cratePos : m_currentStage.cratePosList)
		{
			int index = cratePos.y * m_currentStage.size.x + cratePos.x;

			if (m_currentStage.gridTiles[index] != TileType::Goal)
				return false;
		}

		return true;
	}

	void PlayScene::undo()
	{
		if (m_recordStack.size() <= 1)
			return;

		m_recordStack.pop();

		const StateRecord& previousRecord = m_recordStack.top();

		m_currentStage.playerPos = previousRecord.playerPos;
		m_currentStage.cratePosList = previousRecord.cratePositions;
		m_moveCount = previousRecord.moveCount;

		m_player.SnapToGrid(m_currentStage.playerPos);
	}

	void PlayScene::showDialogBox()
	{
		if (!m_showDialog)
			return;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		ImVec2 windowSize = { 240.0f, 210.0f };
		ImGui::SetNextWindowSize(windowSize);
		ImGui::SetNextWindowPos(
			{
				viewport->WorkPos.x + (viewport->WorkSize.x - windowSize.x) * 0.5f,
				viewport->WorkPos.y + (viewport->WorkSize.y - windowSize.y) * 0.5f
			},
			ImGuiCond_Always
		);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoScrollbar;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 9.0f));

		if (ImGui::Begin("##Title Menu", nullptr, flags))
		{
			float buttonWidth = 202.5f;
			float buttonHeight = 39.0f;
			float windowWidth = ImGui::GetWindowSize().x;

			ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
			if (ImGui::Button("Retry", ImVec2(buttonWidth, buttonHeight)))
			{
				LoadStage(m_stageLevel);
				m_showDialog = false;
			}

			ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
			if (ImGui::Button("Save", ImVec2(buttonWidth, buttonHeight)))
			{
				m_showDialog = false;
			}

			ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
			if (ImGui::Button("Load", ImVec2(buttonWidth, buttonHeight)))
			{
				m_showDialog = false;
			}

			ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
			if (ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight)))
			{
				m_gameContext.changeScene(eSceneType::Title);
				m_showDialog = false;
			}
		}
		ImGui::End();

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();
	}
}