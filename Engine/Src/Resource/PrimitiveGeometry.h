#pragma once
#include "Renderer/Mesh.h"
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

namespace Dive
{
	static void CreateCube(std::vector<Dive::VertexType>* pVertices, std::vector<unsigned int>* pIndices, float width = 1.0f, float height = 1.0f, float depth = 1.0f)
	{
		// fornt
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), height * 0.5f, -(depth * 0.5f)), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, height * 0.5f, -(depth * 0.5f)), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, -(height * 0.5f), -(depth * 0.5f)), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), -(height * 0.5f), -(depth * 0.5f)), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

		pIndices->emplace_back(0);	pIndices->emplace_back(2);	pIndices->emplace_back(3);
		pIndices->emplace_back(0);	pIndices->emplace_back(1);	pIndices->emplace_back(2);

		// bottom
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), -(height * 0.5f), -(depth * 0.5f)), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, -(height * 0.5f), -(depth * 0.5f)), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, -(height * 0.5f), depth * 0.5f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), -(height * 0.5f), depth * 0.5f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));

		pIndices->emplace_back(4);	pIndices->emplace_back(6);	pIndices->emplace_back(7);
		pIndices->emplace_back(4);	pIndices->emplace_back(5);	pIndices->emplace_back(6);

		// back
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), height * 0.5f, depth * 0.5f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, height * 0.5f, depth * 0.5f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, -(height * 0.5f), depth * 0.5f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), -(height * 0.5f), depth * 0.5f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

		pIndices->emplace_back(9);	pIndices->emplace_back(11);	pIndices->emplace_back(10);
		pIndices->emplace_back(9);	pIndices->emplace_back(8);	pIndices->emplace_back(11);

		// top
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), height * 0.5f, depth * 0.5f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, height * 0.5f, depth * 0.5f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, height * 0.5f, -(depth * 0.5f)), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), height * 0.5f, -(depth * 0.5f)), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));

		pIndices->emplace_back(12);	pIndices->emplace_back(14);	pIndices->emplace_back(15);
		pIndices->emplace_back(12);	pIndices->emplace_back(13);	pIndices->emplace_back(14);

		// right
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, height * 0.5f, -(depth * 0.5f)), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, height * 0.5f, depth * 0.5f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, -(height * 0.5f), depth * 0.5f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, -(height * 0.5f), -(depth * 0.5f)), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

		pIndices->emplace_back(16);	pIndices->emplace_back(18);	pIndices->emplace_back(19);
		pIndices->emplace_back(16);	pIndices->emplace_back(17);	pIndices->emplace_back(18);

		// left
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), height * 0.5f, depth * 0.5f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), height * 0.5f, -(depth * 0.5f)), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), -(height * 0.5f), -(depth * 0.5f)), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), -(height * 0.5f), depth * 0.5f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

		pIndices->emplace_back(20);	pIndices->emplace_back(22);	pIndices->emplace_back(23);
		pIndices->emplace_back(20);	pIndices->emplace_back(21);	pIndices->emplace_back(22);
	}

	static void CreatePlane(std::vector<Dive::VertexType>* pVertices, std::vector<unsigned int>* pIndices, unsigned int width = 10, unsigned int depth = 10)
	{
		for (unsigned int z = 0; z < depth + 1; z++)
		{
			for (unsigned int x = 0; x < width + 1; x++)
			{
				pVertices->emplace_back(
					XMFLOAT3(-static_cast<float>(width / 2) + static_cast<float>(x), 0.0f, static_cast<float>(depth / 2) - static_cast<float>(z)),
					XMFLOAT2(static_cast<float>(x) / static_cast<float>(width), static_cast<float>(z) / static_cast<float>(depth)),
					XMFLOAT3(0.0f, 1.0f, 0.0f),
					XMFLOAT3(1.0f, 0.0f, 0.0f));

				if (z < depth && x < width)
				{
					pIndices->emplace_back((z * (width + 1)) + x);
					pIndices->emplace_back((z * (width + 1)) + x + width + 2);
					pIndices->emplace_back((z * (width + 1)) + x + width + 1);

					pIndices->emplace_back((z * (width + 1)) + x);
					pIndices->emplace_back((z * (width + 1)) + x + 1);
					pIndices->emplace_back((z * (width + 1)) + x + width + 2);
				}
			}
		}
	}

	static void CreateQuad(std::vector<Dive::VertexType>* pVertices, std::vector<unsigned int>* pIndices, float width = 1.0f, float height = 1.0f)
	{
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), height * 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, height * 0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(width * 0.5f, -(height * 0.5f), 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(-(width * 0.5f), -(height / 2.0f), 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

		pIndices->emplace_back(0);	pIndices->emplace_back(2);	pIndices->emplace_back(3);
		pIndices->emplace_back(0);	pIndices->emplace_back(1);	pIndices->emplace_back(2);
	}
}