#include "PlayScene.h"
#include "../Graphics/Camera.h"
#include "Core/Window.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/RenderTexture.h"
#include "Input/Input.h"
#include "Core/LogManager.h"
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
		auto graphics = m_gameContext.graphics;

		// App에서 Graphics를 직접 다루는 건 지양해야 한다.
		// Renderer를 통해야 한다.
		{
			Dive::ShaderProgramDesc desc;
			desc.name = "Sokoban";
			desc.vsPath = "Assets/Shaders/Sokoban.hlsl";
			desc.psPath = "Assets/Shaders/Sokoban.hlsl";
			desc.ilType = Dive::eInputLayout::Color;
			Dive::ShaderManager::Get().CreateShaderProgram(graphics, desc);

			desc = {};
			desc.name = "Resolve";
			desc.vsPath = "Assets/Shaders/ResolveScene.hlsl";
			desc.psPath = "Assets/Shaders/ResolveScene.hlsl";
			desc.ilType = Dive::eInputLayout::None;
			Dive::ShaderManager::Get().CreateShaderProgram(graphics, desc);
		}

		m_blockManager.Initialize(graphics);

		m_cbFrame = std::make_unique<Dive::ConstantBuffer<FrameData>>(graphics);
		m_cbObject = std::make_unique<Dive::ConstantBuffer<ObjectData>>(graphics);

		{
			{
				D3D11_TEXTURE2D_DESC desc{};
				desc.Width = graphics->GetWidth();
				desc.Height = graphics->GetHeight();
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.SampleDesc.Count = 1;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.MiscFlags = 0;
				m_rt = std::make_unique<Dive::RenderTexture>(graphics, desc);

				desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				m_dsv = std::make_unique<Dive::RenderTexture>(graphics, desc);
			}

			Dive::RenderTargetDesc rtDesc{};
			rtDesc.RenderTargetView = m_rt->GetRenderTargetView();
			rtDesc.ClearColor[0] = 0.0f;
			rtDesc.ClearColor[1] = 0.0f;
			rtDesc.ClearColor[2] = 0.0f;
			rtDesc.ClearColor[3] = 1.0f;
			rtDesc.AccessType = Dive::eLoadAccessOp::Clear;

			Dive::DepthStencilDesc dsDesc{};
			dsDesc.DepthStencilView = m_dsv->GetDetphStencilView();
			dsDesc.ClearFlags = D3D11_CLEAR_DEPTH;
			dsDesc.AccessType = Dive::eLoadAccessOp::Clear;

			Dive::RenderPassDesc desc{};
			desc.renderTargetDescs.push_back(rtDesc);
			desc.depthStencilDesc = dsDesc;
			m_offScreenResolvePass = desc;
		}

		{
			D3D11_DEPTH_STENCIL_DESC desc{};
			D3D11_DEPTH_STENCILOP_DESC stencilOp{};
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = false;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			stencilOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
			desc.FrontFace = stencilOp;
			desc.BackFace = stencilOp;

			m_dss = graphics->CreateDepthStencilState(desc);
		}

		{
			m_stages.push_back(LevelOne());
			m_stages.push_back(LevelTwo());
			m_stages.push_back(LevelThree());

			LoadStage(2);
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

		auto graphics = m_gameContext.graphics;
		auto input = m_gameContext.input;

		FrameData data = {};
		data.viewProj = DirectX::XMMatrixTranspose(m_gameContext.camera->GetViewMatrix() * m_gameContext.camera->GetProjectionMatrix());
		m_cbFrame->Update(graphics, data);

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
		auto graphics = m_gameContext.graphics;

		graphics->BeginRenderPass(m_offScreenResolvePass);

		graphics->SetDepthStencilState(m_dss.Get(), 1);

		auto sp = Dive::ShaderManager::Get().GetShaderProgram("Sokoban");
		sp->Bind(graphics);

		m_cbFrame->Bind(graphics, Dive::eShaderStage::VS, 0);
		m_cbFrame->Bind(graphics, Dive::eShaderStage::PS, 0);

		graphics->SetTopology(Dive::ePrimitiveTopology::TriangleList);

		// 중앙 정렬
		float offsetX = static_cast<float>(m_currentStage.size.x) * 0.5f;
		float offsetZ = static_cast<float>(m_currentStage.size.y) * 0.5f;

		// 월드 변환 행렬 계산
		auto GetWorldMatrix = [offsetX, offsetZ](int x, int y, float heightOffset) {
			float worldX = static_cast<float>(x) - offsetX + 0.5f;
			float worldZ = static_cast<float>(y) - offsetZ + 0.5f;
			return DirectX::XMMatrixTranslation(worldX, heightOffset, worldZ);
			};

		// Floor, Goal, Wall 그리기
		for (int y = 0; y < m_currentStage.size.y; ++y)
		{
			for (int x = 0; x < m_currentStage.size.x; ++x)
			{
				uint8_t type = m_currentStage.gridTiles[y * m_currentStage.size.x + x];
				auto block = m_blockManager.GetTile(type);
				if (!block) continue;

				int invertedY = m_currentStage.size.y - 1 - y;
				DirectX::XMMATRIX worldMatrix = GetWorldMatrix(x, invertedY, 0.0f);

				ObjectData data = {};
				data.model = DirectX::XMMatrixTranspose(worldMatrix);
				m_cbObject->Update(graphics, data);
				m_cbObject->Bind(graphics, Dive::eShaderStage::VS, 1);

				block->Bind(graphics);
				graphics->DrawIndexed(block->GetIndexCount());
			}
		}

		// 상자 그리기
		auto crateBlock = m_blockManager.GetCrate();
		if (crateBlock)
		{
			for (const auto& cratePos : m_currentStage.cratePosList)
			{
				int invertedY = m_currentStage.size.y - 1 - cratePos.y;
				DirectX::XMMATRIX worldMatrix = GetWorldMatrix(cratePos.x, invertedY, 0.0f);

				ObjectData data = {};
				data.model = DirectX::XMMatrixTranspose(worldMatrix);
				m_cbObject->Update(graphics, data);
				m_cbObject->Bind(graphics, Dive::eShaderStage::VS, 1);

				crateBlock->Bind(graphics);
				graphics->DrawIndexed(crateBlock->GetIndexCount());
			}
		}

		// 플레이어(캡슐) 그리기
		auto playerBlock = m_blockManager.GetPlayer();
		if (playerBlock)
		{
			int invertedY = m_currentStage.size.y - 1 - m_currentStage.playerPos.y;
			DirectX::XMMATRIX worldMatrix = GetWorldMatrix(m_currentStage.playerPos.x, invertedY, 0.0f);

			ObjectData data = {};
			data.model = DirectX::XMMatrixTranspose(worldMatrix);
			m_cbObject->Update(graphics, data);
			m_cbObject->Bind(graphics, Dive::eShaderStage::VS, 1);

			playerBlock->Bind(graphics);
			graphics->DrawIndexed(playerBlock->GetIndexCount());
		}

		graphics->EndRenderPass();
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

			auto srv = m_rt->GetShaderResourceView();
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