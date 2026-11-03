#pragma once
#include <memory>
#include <unordered_map>
#include <DirectXMath.h>
#include "Graphics/Mesh.h"
#include "Types.h"

namespace Sokoban
{
	class Dive::Graphics;

	class BlockManager
	{
	public:

		void Initialize(Dive::Graphics* graphics);

		const Dive::Mesh* GetBlock(eBlockType type) const;

	private:
		std::unique_ptr<Dive::Mesh> createCube(Dive::Graphics* graphics, const DirectX::XMFLOAT3& size, const DirectX::XMFLOAT4& color);
		std::unique_ptr<Dive::Mesh> createFloor(Dive::Graphics* graphics, const DirectX::XMFLOAT4& color);
		std::unique_ptr<Dive::Mesh> createPlayer(Dive::Graphics* graphics, const DirectX::XMFLOAT4& color);
		
	private:
		std::unordered_map<eBlockType, std::unique_ptr<Dive::Mesh>> m_blocks;
	};
}