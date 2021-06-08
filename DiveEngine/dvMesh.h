#pragma once
#include "GraphicsInclude.h"
#include <vector>
#include <string>

namespace Dive
{
	// 구조체가 어울리려나
	struct dvMesh
	{
		std::vector<DirectX::XMFLOAT3> m_positions;
		std::vector<DirectX::XMFLOAT2> m_uv;
		std::vector<DirectX::XMFLOAT2> m_uv1;
		std::vector<DirectX::XMFLOAT4> m_colors;
		std::vector<DirectX::XMFLOAT3> m_normals;
		std::vector<DirectX::XMFLOAT3> m_tangents;

		std::vector<unsigned int> m_indices;

		unsigned int m_vertexCount = 0;
		unsigned int m_indexCount = 0;

		std::string m_name;
	};
}