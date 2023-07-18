#include "Mesh.h"

Mesh::Mesh(std::vector<VertexSkinned> vertices, std::vector<uint32_t> indices)
{
	m_Vertices = vertices;
	m_Indices = indices;
}

// 역시 참고 문서에서는 Shader를 전달받는다.
void Mesh::Draw()
{
	// 메시를 그린다.

	// material을 전달한다.
}