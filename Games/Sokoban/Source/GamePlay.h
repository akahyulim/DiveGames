#pragma once
#include <string>
#include <memory>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Graphics/Types.h"
#include "Graphics/Mesh.h"
#include "Graphics/ConstantBuffer.h"
#include "Stage.h"
#include "Camera.h"
#include "MeshManager.h"

#include "Input/Input.h"

namespace Sokoban
{
	class Dive::Graphics;
	//class Dive::Input;

	struct FrameData
	{
		DirectX::XMMATRIX viewProj;
	};

	struct ObjectData
	{
		DirectX::XMMATRIX model;
	};

	class GamePlay
	{
	public:
		GamePlay(Dive::Graphics* graphics);
		~GamePlay();

		void LoadStage(const Stage& stage);
		void LoadStage(const std::string& filepath);

		void Update(Dive::Graphics* graphics, Dive::Input* input);
		void Render(Dive::Graphics* graphics) const;

		int FindCrateAt(int x, int y);

	private:
		void checkAndMovePlayer(int nextX, int nextY, int dx, int dy);
		bool checkGameClear();

		void firstStage();
		void secondStage();
		void thirdStage();

	private:
		Stage m_currentStage;
		Camera m_camera;

		MeshManager m_blockManager;

		std::unique_ptr<Dive::ConstantBuffer<FrameData>> m_cbFrame;
		std::unique_ptr<Dive::ConstantBuffer<ObjectData>> m_cbObject;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_dss;
	};
}