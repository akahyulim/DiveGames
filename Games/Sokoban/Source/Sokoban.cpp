#include "Sokoban.h"
#include "Graphics/Graphics.h"
#include "Graphics/ShaderManager.h"
//#include "Input/Input.h"
#include "ImGui/ImGuiManager.h"

namespace Sokoban
{
	Sokoban::Sokoban(Dive::Graphics* graphics, Dive::Input* input)
		: m_graphics(graphics),
		m_input(input)
	{
		{
			Dive::ShaderProgramDesc desc;
			desc.name = "Sokoban";
			desc.vsPath = "Assets/Shaders/Sokoban.hlsl";
			desc.psPath = "Assets/Shaders/Sokoban.hlsl";
			desc.ilType = Dive::eInputLayout::Color;
			Dive::ShaderManager::Get().CreateShaderProgram(m_graphics, desc);
		}

		m_camera.SetPerspective(DirectX::XM_PIDIV4, 800.0f / 600.0f, 0.1f, 100.0f);
		m_camera.LookAt({ 0.0f, 30.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });

		m_blockManager.Initialize(m_graphics);
		m_cbFrame = std::make_unique<Dive::ConstantBuffer<FrameData>>(m_graphics);
		m_cbObject = std::make_unique<Dive::ConstantBuffer<ObjectData>>(m_graphics);

		{
			D3D11_DEPTH_STENCIL_DESC desc{};
			D3D11_DEPTH_STENCILOP_DESC stencilOp{};

			// 1. Default (기본 뎁스 읽기/쓰기)
			ZeroMemory(&desc, sizeof(desc));
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

			// 1. 이동 방향 벡터 초기화 (X, Y)
			int dx = 0;
			int dy = 0;

			// 2. 입력 체크 (VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN 또는 WASD)
			// DirectInput이나 Win32 가상 키 코드 기준에 맞춰 설정하세요.
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
				dy = -1; // 화면 좌표계나 배열 인덱스 방향에 따라 -1 또는 +1
			}
			else if (m_input->KeyDown(DIK_DOWN) || m_input->KeyDown(DIK_S))
			{
				dy = 1;
			}

			// 3. 입력이 발생했다면 그리드 로직 처리
			if (dx != 0 || dy != 0)
			{
				int nextX = m_currentStage.playerPos.x + dx;
				int nextY = m_currentStage.playerPos.y + dy;

				// [핵심 로직] 이동하려는 칸의 상태 검사 (벽인지, 상자인지 등)
				checkAndMovePlayer(nextX, nextY, dx, dy);
			}
		}
	}

	void Sokoban::Render()
	{
		m_graphics->SetDepthStencilState(m_dss.Get(), 1);

		auto sp = Dive::ShaderManager::Get().GetShaderProgram("Sokoban");
		sp->Bind(m_graphics);

		m_cbFrame->Bind(m_graphics, Dive::eShaderStage::VS, 0);
		m_cbFrame->Bind(m_graphics, Dive::eShaderStage::PS, 0);

		m_graphics->SetTopology(Dive::ePrimitiveTopology::TriangleList);

		// 📌 1. 중앙 정렬을 위한 오프셋 계산 (전체 크기의 절반)
		float offsetX = static_cast<float>(m_currentStage.width) * 0.5f;
		float offsetZ = static_cast<float>(m_currentStage.height) * 0.5f;

		// 공통으로 쓸 변환 람다 함수 (그리드 (x, y)와 Y축 높이를 받아 월드 행렬 반환)
		auto GetWorldMatrix = [offsetX, offsetZ](int x, int y, float heightOffset) {
			float worldX = static_cast<float>(x) - offsetX + 0.5f; // 타일 중앙 정렬용 +0.5
			float worldZ = static_cast<float>(y) - offsetZ + 0.5f;
			return DirectX::XMMatrixTranslation(worldX, heightOffset, worldZ);
			};

		// ==========================================
		// 2. 배경 레이어 렌더링 (Floor, Wall, Goal)
		// ==========================================
		for (int y = 0; y < m_currentStage.height; ++y)
		{
			for (int x = 0; x < m_currentStage.width; ++x)
			{
				uint8_t type = m_currentStage.gridTiles[y * m_currentStage.width + x];
				auto block = m_blockManager.GetTile(type);
				if (!block) continue;

				int invertedY = m_currentStage.height - 1 - y;
				DirectX::XMMATRIX worldMatrix = GetWorldMatrix(x, invertedY, 0.0f);

				ObjectData data = {};
				data.model = DirectX::XMMatrixTranspose(worldMatrix);
				m_cbObject->Update(m_graphics, data);
				m_cbObject->Bind(m_graphics, Dive::eShaderStage::VS, 1);

				block->Bind(m_graphics);
				m_graphics->DrawIndexed(block->GetIndexCount());
			}
		}

		// ==========================================
		// 3. 상자(Crates) 렌더링
		// ==========================================
		auto crateBlock = m_blockManager.GetCrate();
		if (crateBlock)
		{
			for (const auto& cratePos : m_currentStage.cratePosList)
			{
				// 배경과 동일하게 Y축 반전 적용!
				int invertedY = m_currentStage.height - 1 - cratePos.y;
				DirectX::XMMATRIX worldMatrix = GetWorldMatrix(cratePos.x, invertedY, 0.0f); // 필요시 높이 오프셋(예: 0.4f) 복원

				ObjectData data = {};
				data.model = DirectX::XMMatrixTranspose(worldMatrix);
				m_cbObject->Update(m_graphics, data);
				m_cbObject->Bind(m_graphics, Dive::eShaderStage::VS, 1);

				crateBlock->Bind(m_graphics);
				m_graphics->DrawIndexed(crateBlock->GetIndexCount());
			}
		}

		// ==========================================
		// 4. 플레이어(Player) 렌더링
		// ==========================================
		auto playerBlock = m_blockManager.GetPlayer();
		if (playerBlock)
		{
			// 배경과 동일하게 Y축 반전 적용!
			int invertedY = m_currentStage.height - 1 - m_currentStage.playerPos.y;
			DirectX::XMMATRIX worldMatrix = GetWorldMatrix(m_currentStage.playerPos.x, invertedY, 0.0f); // 필요시 높이 오프셋(예: 0.65f) 복원

			ObjectData data = {};
			data.model = DirectX::XMMatrixTranspose(worldMatrix);
			m_cbObject->Update(m_graphics, data);
			m_cbObject->Bind(m_graphics, Dive::eShaderStage::VS, 1);

			playerBlock->Bind(m_graphics);
			m_graphics->DrawIndexed(playerBlock->GetIndexCount());
		}
	}

	void Sokoban::RenderUI()
	{
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
		// m_crates는 관리 중인 상자들의 리스트 (std::vector<Crate>)
		for (size_t i = 0; i < m_currentStage.cratePosList.size(); ++i)
		{
			if (m_currentStage.cratePosList[i].x == x && m_currentStage.cratePosList[i].y == y)
			{
				return static_cast<int>(i); // 상자를 찾았다면 벡터의 인덱스 반환
			}
		}

		return -1; // 해당 좌표에 상자가 없음
	}

	//void Sokoban::OnWindowEvent(const Dive::WindowEventData& data)
	//{
		//if(GetImGuiManager()->IsVisible())
		//	GetImGuiManager()->HandleWindowMessage(data);
	//}

	void Sokoban::checkAndMovePlayer(int nextX, int nextY, int dx, int dy)
	{
		// 1. 맵 범위를 벗어나는지 체크
		if (nextX < 0 || nextX >= m_currentStage.width || nextY < 0 || nextY >= m_currentStage.height)
		{
			return; // 맵 밖으로 이동 불가
		}

		// 2. 이동하려는 칸의 타일 타입 확인
		uint8_t nextTileType = m_currentStage.gridTiles[nextY * m_currentStage.width + nextX];

		// Case A: 벽(Wall)인 경우 -> 이동 불가
		if (nextTileType == TileType::Empty || nextTileType == TileType::Wall)
		{
			return;
		}

		// Case B: 상자(Crate)가 있는 경우 -> 상자를 밀 수 있는지 검사
		int crateIndex = FindCrateAt(nextX, nextY); // 해당 좌표에 상자가 있는지 찾는 함수 (별도 관리 중이시라면 그에 맞게 수정)
		if (crateIndex != -1) // 상자가 존재함!
		{
			// 상자가 밀려날 그 다음 칸 좌표
			int nextCrateX = nextX + dx;
			int nextCrateY = nextY + dy;

			// 2-1. 상자가 밀려날 곳이 맵 밖이거나, 또 벽이거나, 혹은 다른 상자가 있다면 -> 밀기 불가
			if (nextCrateX < 0 || nextCrateX >= m_currentStage.width ||
				nextCrateY < 0 || nextCrateY >= m_currentStage.height)
			{
				return;
			}

			uint8_t crateDestinationType = m_currentStage.gridTiles[nextCrateY * m_currentStage.width + nextCrateX];
			if (crateDestinationType == TileType::Empty || crateDestinationType == TileType::Wall || FindCrateAt(nextCrateX, nextCrateY) != -1)
			{
				return; // 막혀서 못 밀음
			}

			// 2-2. 상자를 한 칸 이동시킴
			m_currentStage.cratePosList[crateIndex].x = nextCrateX;
			m_currentStage.cratePosList[crateIndex].y = nextCrateY;
		}

		// Case C: 빈 바닥(또는 목표 지점 등)인 경우 -> 플레이어 이동 완료!
		m_currentStage.playerPos.x = nextX;
		m_currentStage.playerPos.y = nextY;
		m_moveCount++;

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
			int index = cratePos.y * m_currentStage.width + cratePos.x;

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