#pragma once
#include <memory>
#include <string>
#include <stack>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Graphics/Types.h"
#include "Graphics/Mesh.h"
#include "Graphics/ConstantBuffer.h"

#include "Core/Application.h"
#include "Camera.h"
#include "Stage.h"
#include "MeshManager.h"

namespace Sokoban
{
	class Dive::Graphics;
	class Dive::Input;

	struct FrameData
	{
		DirectX::XMMATRIX viewProj;
	};

	struct ObjectData
	{
		DirectX::XMMATRIX model;
	};

	// GridPosition는 DirectX::XMINT가 나을 듯하다.
	struct StateRecord
	{
		GridPosition playerPos;
		std::vector<GridPosition> cratePositions;
		int moveCount;
	};

	class Sokoban : public Dive::Application
	{
	public:
		Sokoban() = default;
		~Sokoban() override;

		void LoadStage(const Stage& stage);
		void LoadStage(const std::string& filepath);

		int FindCrateAt(int x, int y);

	protected:
		void Initialize() override;
		void Update(float deltaTime) override;
		void Render() override;
		void Shutdown() override;
	
	private:
		void checkAndMovePlayer(int nextX, int nextY, int dx, int dy);
		bool checkGameClear();

		void undo();

	private:
		Stage m_currentStage;
		Camera m_camera;

		MeshManager m_blockManager;

		int m_moveCount = 0;
		std::stack<StateRecord> m_recordStack;

		std::unique_ptr<Dive::ConstantBuffer<FrameData>> m_cbFrame;
		std::unique_ptr<Dive::ConstantBuffer<ObjectData>> m_cbObject;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_dss;
	};
}