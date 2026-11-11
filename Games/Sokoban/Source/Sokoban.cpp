#include "Sokoban.h"
#include "Graphics/Graphics.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/RenderPass.h"
//#include "Input/Input.h"
//#include "ImGui/ImGuiManager.h"
#include <Graphics/TextureManager.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace Sokoban
{
	Sokoban::Sokoban(Dive::Graphics* graphics, Dive::Input* input, Dive::ImGuiManager* imGuiMng)
		: m_graphics(graphics),
		m_input(input),
		m_imGuiMng(imGuiMng)
	{
		{
			Dive::ShaderProgramDesc desc;
			desc.name = "Sokoban";
			desc.vsPath = "Assets/Shaders/Sokoban.hlsl";
			desc.psPath = "Assets/Shaders/Sokoban.hlsl";
			desc.ilType = Dive::eInputLayout::Color;
			Dive::ShaderManager::Get().CreateShaderProgram(m_graphics, desc);

			desc = {};
			desc.name = "Resolve";
			desc.vsPath = "Assets/Shaders/ResolveScene.hlsl";
			desc.psPath = "Assets/Shaders/ResolveScene.hlsl";
			desc.ilType = Dive::eInputLayout::None;
			Dive::ShaderManager::Get().CreateShaderProgram(m_graphics, desc);
		}

		m_camera.SetPerspective(DirectX::XM_PIDIV4, 800.0f / 600.0f, 0.1f, 100.0f);
		m_camera.LookAt({ 0.0f, 30.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });

		m_blockManager.Initialize(m_graphics);

		m_cbFrame = std::make_unique<Dive::ConstantBuffer<FrameData>>(m_graphics);
		m_cbObject = std::make_unique<Dive::ConstantBuffer<ObjectData>>(m_graphics);
		
		{
			// TextureManager로 텍스쳐를 읽은 후
			// ImGui의 image 출력 다시 테스트
			{
				//auto handle = Dive::TextureManager::Get().LoadTexture("Assets/Textures/DokeV.jpeg");
			}

			{
				D3D11_TEXTURE2D_DESC desc{};
				desc.Width = m_graphics->GetWidth();
				desc.Height = m_graphics->GetHeight();
				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				desc.MipLevels = 1;
				desc.ArraySize = 1;
				desc.SampleDesc.Count = 1;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.MiscFlags = 0;
				m_rt = std::make_unique<Dive::RenderTexture>(m_graphics, desc);
				
				desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				m_ds = std::make_unique<Dive::RenderTexture>(m_graphics, desc);
			}

			Dive::RenderTargetDesc rtDesc{};
			rtDesc.RenderTargetView = m_rt->GetRenderTargetView();
			rtDesc.ClearColor[0] = 0.0f;
			rtDesc.ClearColor[1] = 0.0f;
			rtDesc.ClearColor[2] = 0.0f;
			rtDesc.ClearColor[3] = 0.0f;
			rtDesc.AccessType = Dive::eLoadAccessOp::Clear;

			Dive::DepthStencilDesc dsDesc{};
			dsDesc.DepthStencilView = m_ds->GetDetphStencilView();
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

			m_dss = m_graphics->CreateDepthStencilState(desc);
		}

		LoadStage(LevelOne());
	}

	Sokoban::~Sokoban() = default;

	// 게임 플레이 타임은 deltaTime을 누적해서 계산한다.
	void Sokoban::Update(float deltaTime)
	{
		if (checkGameClear())
			return;

		FrameData data = {};
		data.viewProj = DirectX::XMMatrixTranspose(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
		m_cbFrame->Update(m_graphics, data);

		{
			if (m_input->KeyDown(DIK_R))
			{
				LoadStage(LevelOne());
				return;
			}

			if (m_input->KeyDown(DIK_U) || m_input->KeyDown(DIK_BACKSPACE))
			{
				undo();
				return;
			}

			int dx = 0;
			int dy = 0;

			if (m_input->KeyDown(DIK_LEFT) || m_input->KeyDown(DIK_A))
			{
				dx = -1;
			}
			else if (m_input->KeyDown(DIK_RIGHT) || m_input->KeyDown(DIK_D))
			{
				dx = 1;
			}
			else if (m_input->KeyDown(DIK_UP) || m_input->KeyDown(DIK_W))
			{
				dy = -1;
			}
			else if (m_input->KeyDown(DIK_DOWN) || m_input->KeyDown(DIK_S))
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

	// m_offScreenResolvePass는 m_rt, m_ds에 그리는 거다.
	void Sokoban::Render()
	{
		m_graphics->BeginRenderPass(m_offScreenResolvePass);

		m_graphics->SetDepthStencilState(m_dss.Get(), 1);

		auto sp = Dive::ShaderManager::Get().GetShaderProgram("Sokoban");
		sp->Bind(m_graphics);

		m_cbFrame->Bind(m_graphics, Dive::eShaderStage::VS, 0);
		m_cbFrame->Bind(m_graphics, Dive::eShaderStage::PS, 0);

		m_graphics->SetTopology(Dive::ePrimitiveTopology::TriangleList);

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
				m_cbObject->Update(m_graphics, data);
				m_cbObject->Bind(m_graphics, Dive::eShaderStage::VS, 1);

				block->Bind(m_graphics);
				m_graphics->DrawIndexed(block->GetIndexCount());
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
				m_cbObject->Update(m_graphics, data);
				m_cbObject->Bind(m_graphics, Dive::eShaderStage::VS, 1);

				crateBlock->Bind(m_graphics);
				m_graphics->DrawIndexed(crateBlock->GetIndexCount());
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
			m_cbObject->Update(m_graphics, data);
			m_cbObject->Bind(m_graphics, Dive::eShaderStage::VS, 1);

			playerBlock->Bind(m_graphics);
			m_graphics->DrawIndexed(playerBlock->GetIndexCount());
		}

		m_graphics->EndRenderPass();
	}

	// m_rt에 그린 걸 srv로 13번에 할당한 후 백버퍼에 그린다.
	// imgui를 사용한다면 resolve가 필요없다.
	// img에 m_rt의 srv를 전달하여 그리고
	// imgui 자체는 backbuffer에 그린다.
	void Sokoban::RenderUI()
	{	
		m_graphics->ClearBackbuffer();
		m_graphics->SetBackbuffer();

		m_imGuiMng->Begin();
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

				//auto srv = Dive::TextureManager::Get().GetTextureViewByPath("Assets/Textures/DokeV.jpeg");
				auto srv = m_rt->GetShaderResourceView();
				ImGui::Image((ImTextureID)srv, viewportSize);
			}

			ImGui::End();
		}
		m_imGuiMng->End();

		//auto srv = m_rt->GetShaderResourceView();
		//m_graphics->SetShaderResourceView(Dive::eShaderStage::PS, 13, &srv);
		//Dive::ShaderManager::Get().GetShaderProgram("Resolve")->Bind(m_graphics);
		//m_graphics->SetTopology(Dive::ePrimitiveTopology::TriangleStrip);
		//m_graphics->SetVertexBuffer(nullptr);
		//m_graphics->Draw(4);
	}

	void Sokoban::Shutdown()
	{
	}

	void Sokoban::LoadStage(const Stage& stage)
	{
		m_currentStage = stage;
		
		std::stack<StateRecord>().swap(m_recordStack);

		StateRecord record;
		record.playerPos = m_currentStage.playerPos;
		record.cratePositions = m_currentStage.cratePosList;
		record.moveCount = m_moveCount = 0;
		m_recordStack.push(record);
	}

	void Sokoban::LoadStage(const std::string& filepath)
	{
	}

	int Sokoban::FindCrateAt(int x, int y)
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

	//void Sokoban::OnWindowEvent(const Dive::WindowEventData& data)
	//{
		//if(GetImGuiManager()->IsVisible())
		//	GetImGuiManager()->HandleWindowMessage(data);
	//}

	void Sokoban::checkAndMovePlayer(int nextX, int nextY, int dx, int dy)
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

	bool Sokoban::checkGameClear()
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

	void Sokoban::undo()
	{
		if (m_recordStack.size() <= 1)
			return;

		m_recordStack.pop();

		const StateRecord& previousRecord = m_recordStack.top();

		m_currentStage.playerPos = previousRecord.playerPos;
		m_currentStage.cratePosList = previousRecord.cratePositions;
		m_moveCount = previousRecord.moveCount;
	}
}