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
	// Mesh�� ��� ������ �������� �����ؾ� �Ѵ�.
	// ���� Editor�� Sandbox �� �� static���� �����Ͽ���.
	// MeshRenderer���� �����ϴ� ���� ���� ���� �� ����.
	class dvMesh
	{
	public:
		dvMesh() = default;
		~dvMesh() { Clear(); }

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