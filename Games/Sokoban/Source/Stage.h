#pragma once
#include <vector>

namespace Sokoban
{
	class Mesh;

	enum class eTileType
	{
		Empty,
		Floor,
		Wall,
		Goal,
		Crate,
		Player
	};

	struct GridPosition
	{
		int x;
		int y;
	};
	
	struct Stage
	{
		int width;
		int height;
		std::vector<eTileType> m_gridTiles;

		GridPosition m_playerPosition;
		std::vector<GridPosition> m_cratePositions;
	};
}