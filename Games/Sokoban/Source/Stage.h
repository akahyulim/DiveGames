#pragma once
#include <vector>
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
		std::vector<eBlockType> gridTiles;

		GridPosition playerPos;
		std::vector<GridPosition> cratePosList;
	};
}