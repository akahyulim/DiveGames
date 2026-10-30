#include "GamePlay.h"

namespace Sokoban
{
	GamePlay::GamePlay()
	{
		m_camera.SetPerspective(DirectX::XM_PIDIV4, 800.0f / 600.0f, 0.1f, 100.0f);
		m_camera.LookAt({ 5.0f, 12.0f, -10.0f }, { 5.0f, 0.0f, 5.0f }, { 0.0f, 1.0f, 0.0f });

		// 셰이더 생성은 ShaderManager로 생성해 ShaderPrograme으로 받는다.
		// 기존 Initialize가 무겁기 때문에 필요한 것만 생성하는 기능을 추가해야 한다.
	}

	GamePlay::~GamePlay() = default;

	void GamePlay::TestStage()
	{
	}
	
	void GamePlay::LoadStage(const Stage& stage)
	{
		m_currentStage = stage;
	}

	void GamePlay::LoadStage(const std::string& filepath)
	{
	}
	
	void GamePlay::Update()
	{
	}
	
	void GamePlay::Render() const
	{
		// 셰이더를 바인딩해야 한다.
		// 여기서도 ShaderProgram를 사용하면 된다.

		for (int y = 0; y < m_currentStage.height; ++y)
		{
			for (int x = 0; x < m_currentStage.width; ++x)
			{
				//eTileType tile = m_currentStage.m_gridTiles[y * m_currentStage.width + x];

				//tileType에 맞는 Mesh Draw
			}
		}
	}
}
