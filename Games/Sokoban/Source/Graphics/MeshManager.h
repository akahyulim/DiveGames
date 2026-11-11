#pragma once
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <DirectXMath.h>
#include "Graphics/Mesh.h"

namespace Sokoban
{
	class Dive::Graphics;

	class MeshManager
	{
	public:

		void Initialize(Dive::Graphics* graphics);

		const Dive::Mesh* GetTile(uint8_t type) const;
		const Dive::Mesh* GetWall() const { return m_wall.get(); }
		const Dive::Mesh* GetFloor() const { return m_floor.get(); }
		const Dive::Mesh* GetGoal() const { return m_goal.get(); }
		const Dive::Mesh* GetCrate() const { return m_crate.get(); }
		const Dive::Mesh* GetPlayer() const { return m_player.get(); }

	private:
		std::unique_ptr<Dive::Mesh> createCube(Dive::Graphics* graphics, const DirectX::XMFLOAT3& size, const DirectX::XMFLOAT4& color);
		std::unique_ptr<Dive::Mesh> createFloor(Dive::Graphics* graphics, const DirectX::XMFLOAT4& color);
		std::unique_ptr<Dive::Mesh> createPlayer(Dive::Graphics* graphics, const DirectX::XMFLOAT4& color);
		
	private:
		std::unique_ptr<Dive::Mesh> m_wall;
		std::unique_ptr<Dive::Mesh> m_floor;
		std::unique_ptr<Dive::Mesh> m_goal;
		std::unique_ptr<Dive::Mesh> m_crate;
		std::unique_ptr<Dive::Mesh> m_player;
	};
}