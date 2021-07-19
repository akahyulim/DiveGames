#pragma once
#include "GraphicsInclude.h"
#include "VertexTypes.h"
#include <vector>
#include <string>

namespace dive
{	
	enum eMeshType
	{
		Static,
		Skinned,
	};

	// �ϴ� static mesh�� �����ϰ�, ���� �и��ϴ� ����� ����غ���.
	// skinned mesh�� ��쿡�� �ᱹ�� ���� mesh�� �������� ���� ����.
	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh() { Clear(); }

		void Clear();

		// object�� ��ӽ�Ű�� ���� ���� �� ����.
		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		unsigned int GetVertexCount() const { return static_cast<unsigned int>(m_Vertices.size()); }
		std::vector<VertexType_PosTexNorTan>& GetVertices() { return m_Vertices; }
		void SetVertices(const std::vector<VertexType_PosTexNorTan>& vertices) { m_Vertices = vertices; }

		unsigned int GetIndexCount() const { return static_cast<unsigned int>(m_Indices.size()); }
		std::vector<unsigned int>& GetIndices() { return m_Indices; }
		void SetIndices(const std::vector<unsigned int>& indices) { m_Indices = indices; }

	private:
		std::string m_Name = "";

		std::vector<VertexType_PosTexNorTan> m_Vertices;
		std::vector<unsigned int> m_Indices;
	};
}