#include "Mesh.h"

Mesh::Mesh(std::vector<VertexSkinned> vertices, std::vector<uint32_t> indices)
{
	m_Vertices = vertices;
	m_Indices = indices;
}