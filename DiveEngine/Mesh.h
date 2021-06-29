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

		std::string GetName() const { return mName; }
		void SetName(const std::string& name) { mName = name; }

		unsigned int GetVertexCount() const { return static_cast<unsigned int>(mVertices.size()); }
		std::vector<VertexType_PosTexNorTan>& GetVertices() { return mVertices; }
		void SetVertices(const std::vector<VertexType_PosTexNorTan>& vertices) { mVertices = vertices; }

		unsigned int GetIndexCount() const { return static_cast<unsigned int>(mIndices.size()); }
		std::vector<unsigned int>& GetIndices() { return mIndices; }
		void SetIndices(const std::vector<unsigned int>& indices) { mIndices = indices; }

	private:
		std::string mName = "";

		std::vector<VertexType_PosTexNorTan> mVertices;
		std::vector<unsigned int> mIndices;
	};
}