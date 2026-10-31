#pragma once
#include <string>
#include <memory>
#include "Graphics/Types.h"
#include "Graphics/Mesh.h"
#include "Stage.h"
#include "Camera.h"

namespace Sokoban
{
	class Dive::Graphics;

	class GamePlay
	{
	public:
		GamePlay();
		~GamePlay();

		void TestStage();

		void LoadStage(const Stage& stage);
		void LoadStage(const std::string& filepath);

		void Update();

		void Render(Dive::Graphics* graphics) const;

		void CreateWallMesh(const Dive::Color& color);
		void CreateCrateMesh(const Dive::Color& color);

	private:


	private:
		Stage m_currentStage;
		Camera m_camera;

		std::unique_ptr<Dive::Mesh> m_wallMesh;
		std::unique_ptr<Dive::Mesh> m_floorMesh;
		std::unique_ptr<Dive::Mesh> m_goalMesh;
		std::unique_ptr<Dive::Mesh> m_crateMesh;
		std::unique_ptr<Dive::Mesh> m_playerMesh;
	};
}