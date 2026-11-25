#include "PlayScene2D.h"
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
	PlayScene2D::PlayScene2D(const GameContext& context)
		: Scene(context)
	{
	}

	PlayScene2D::~PlayScene2D()
	{
	}

	void PlayScene2D::Initialize()
	{
		{
			auto camera = m_gameContext.camera;// ->LookAt({ 0.0f, 0.0f, -30.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
			// 예시: (x, y) 위치에 있는 2D 카메라
			camera->LookAt(
				{ 0.0f, 0.0f, -10.0f }, // eye
				{ 0.0f, 0.0f,   0.0f }, // target
				{ 0.0f, 1.0f,   0.0f }  // up (+Y가 위쪽)
			);
			camera->SetOrthographic(1280, 720, 0.1f, 100.0f);
		}

		{
			m_stages.push_back(LevelOne());
			m_stages.push_back(LevelTwo());
			m_stages.push_back(LevelThree());

			LoadStage(2);
		}
	}

	void PlayScene2D::Cleanup()
	{
		m_stages.clear();
	}

	void PlayScene2D::Update(float deltaTime)
	{
		if (checkGameClear())
		{
			// 대화상자가 뜨고 Next 버튼을 누르면 넘어가도록 하자.
			// 축하 메시지와 함께 점수를 보여줄 수도 있다.
			LoadStage(++m_stageLevel);
		}

		auto graphics = m_gameContext.graphics;
		auto input = m_gameContext.input;

		FrameData data = {};
		data.viewProj = DirectX::XMMatrixTranspose(m_gameContext.camera->GetViewMatrix() * m_gameContext.camera->GetProjectionMatrix());
		//m_cbFrame->Update(graphics, data);

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

	void PlayScene2D::Render()
	{
		if (m_gameContext.renderer == nullptr)
			return;

		m_gameContext.renderer->BeginScene(
			DirectX::XMMatrixTranspose(
				m_gameContext.camera->GetViewMatrix() *
				m_gameContext.camera->GetProjectionMatrix()));

		m_gameContext.renderer->DrawQuad({ 0.0f, 0.0f }, { 600.0f, 598.0f },
			Dive::TextureManager::Get().LoadTexture("Assets/Textures/kenney_sokoban-pack/Spritesheet/sokoban_spritesheet.png"));
		
		m_gameContext.renderer->EndScene();
	}

	void PlayScene2D::RenderUI()
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

	void PlayScene2D::LoadStage(int level)
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
	}

	void PlayScene2D::LoadStage(const std::string& filepath)
	{
	}

	int PlayScene2D::FindCrateAt(int x, int y)
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

	void PlayScene2D::checkAndMovePlayer(int nextX, int nextY, int dx, int dy)
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

		m_currentStage.playerPos.x = nextX;
		m_currentStage.playerPos.y = nextY;
		m_moveCount++;

		// 이동 결과 저장
		StateRecord record;
		record.playerPos = m_currentStage.playerPos;
		record.cratePositions = m_currentStage.cratePosList;
		record.moveCount = m_moveCount;
		m_recordStack.push(record);
	}

	bool PlayScene2D::checkGameClear()
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

	void PlayScene2D::undo()
	{
		if (m_recordStack.size() <= 1)
			return;

		m_recordStack.pop();

		const StateRecord& previousRecord = m_recordStack.top();

		m_currentStage.playerPos = previousRecord.playerPos;
		m_currentStage.cratePosList = previousRecord.cratePositions;
		m_moveCount = previousRecord.moveCount;
	}

	void PlayScene2D::showDialogBox()
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