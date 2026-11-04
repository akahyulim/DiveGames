#pragma once
#include <vector>
#include <cstdint>
#include "Types.h"

namespace Sokoban
{
	struct GridPosition
	{
		int x;
		int y;
	};
	
	struct Stage
	{
		int width;
		int height;
		std::vector<uint8_t> gridTiles;

		GridPosition playerPos;
		std::vector<GridPosition> cratePosList;
	};
}