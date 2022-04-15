#pragma once
#include "DiveEngine.h"

using namespace DirectX;

namespace Utilities::Geometry
{
	static void CreateCube(std::vector<Dive::VertexType>* pVertices, std::vector<unsigned int>* pIndices)
	{
		// fornt
		
		// back

		// left

		// right

		// top

		// bottom
	}

	// 10 x 10
	static void CreatePlane(std::vector<Dive::VertexType>* pVertices, std::vector<unsigned int>* pIndices, unsigned int width = 10, unsigned int depth = 10)
	{
		for (unsigned int z = 0; z < depth; z++)
		{
			for (unsigned int x = 0; x < width + 1; x++)
			{
				pVertices->emplace_back(
					XMFLOAT3(-static_cast<float>(width/2) + static_cast<float>(x), 0.0f, static_cast<float>(depth/2) - static_cast<float>(z)),
					XMFLOAT2(static_cast<float>(x) / static_cast<float>(width), static_cast<float>(z) / static_cast<float>(depth)),
					XMFLOAT3(0.0f, 1.0f, 0.0f),
					XMFLOAT3(1.0f, 0.0f, 0.0f));
			}
		}

		// 일단 이 것부터 수정
		for (unsigned int i = 0; i < depth; i++)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				pIndices->emplace_back(i + j);	pIndices->emplace_back(i + width + 1);	pIndices->emplace_back(i + width);
				pIndices->emplace_back(i + j);	pIndices->emplace_back(i + j + 1);	pIndices->emplace_back(i + width + 1);
			}
		}

		for (auto vertex : *pVertices)
		{
			//DV_APP_INFO("vertex: {0:f}, {1:f}, {2:f}", vertex.position[0], vertex.position[1], vertex.position[2]);
		}

		for (auto index : *pIndices)
		{
			DV_APP_INFO("indices: {:d}", index);
		}

		DV_APP_INFO("index count: {:d}", static_cast<int>(pIndices->size()));
	}

	static void CreateQuad(std::vector<Dive::VertexType>* pVertices, std::vector<unsigned int>* pIndices)
	{
		pVertices->emplace_back(XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		pVertices->emplace_back(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

		pIndices->emplace_back(0);	pIndices->emplace_back(2);	pIndices->emplace_back(3);
		pIndices->emplace_back(0);	pIndices->emplace_back(1);	pIndices->emplace_back(2);
	}
}