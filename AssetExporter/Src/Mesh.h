#pragma once
#include "Dive.h"

struct VertexSkinned
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT2 texCoords;
	int boneIDs[4] = { -1, -1, -1, -1 };
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class Mesh
{
public:
	Mesh(std::vector<VertexSkinned> vertices, std::vector<uint32_t> indices);

	std::vector<VertexSkinned> GetVertices() const { return m_Vertices; }

private:
private:
	std::vector<VertexSkinned> m_Vertices;
	std::vector<uint32_t> m_Indices;
};