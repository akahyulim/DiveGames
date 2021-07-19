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

	// 일단 static mesh만 구현하고, 추후 분리하는 방법을 고민해보자.
	// skinned mesh의 경우에도 결국엔 단일 mesh로 구성함을 잊지 말자.
	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh() { Clear(); }

		void Clear();

		// object를 상속시키는 편이 나을 것 같다.
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