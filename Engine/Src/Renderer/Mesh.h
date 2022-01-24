#pragma once
#include "Graphics/GraphicsInclude.h"
#include "VertexTypes.h"
#include <vector>
#include <string>

namespace Dive
{	
	enum eMeshType
	{
		Static,
		Skinned,
	};

	// �ƹ��� �����ص� �갡 ���۸� ������ �� �� ����.
	// �׷��� ���۸� �����ٸ� offset + count�� ��Ī�Ǵ� Transform�� ���� GameObject�� �����ؾ� �Ѵ�.
	// �� GameObject�� ��쿣 �̸����� ������� Ŀ���� �Ǳ� �� �� ����.

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

		//= �߰� ==========================================
		bool CreateGeometryBuffers(ID3D11Device* pDevice);

		unsigned int GetVertexStride() const { return m_VertexStride; }

		unsigned int GetVertexBufferOffset() const { return m_VertexBufferOffset; }
		unsigned int GetIndexBufferOffset() const { return m_IndexBufferOffset; }

		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }

	private:
		std::string m_Name = "";

		std::vector<VertexType_PosTexNorTan> m_Vertices;
		std::vector<unsigned int> m_Indices;

		//= �߰� ==========================================
		unsigned int m_VertexStride;
		unsigned int m_VertexBufferOffset;
		unsigned int m_IndexBufferOffset;

		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
	};
}