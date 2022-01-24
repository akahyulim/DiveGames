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

	// 아무리 생각해도 얘가 버퍼를 가져야 할 것 같다.
	// 그런데 버퍼를 가진다면 offset + count와 매칭되는 Transform을 가진 GameObject도 관리해야 한다.
	// 뭐 GameObject의 경우엔 이름으로 어느정도 커버가 되긴 할 거 같다.

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

		//= 추가 ==========================================
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

		//= 추가 ==========================================
		unsigned int m_VertexStride;
		unsigned int m_VertexBufferOffset;
		unsigned int m_IndexBufferOffset;

		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
	};
}