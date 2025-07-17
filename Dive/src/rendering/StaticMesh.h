#pragma once
#include "resource/Resource.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;
	class BoundingBox;
	class Shader;

	enum class eSourceType
	{
		None,
		File,
		Factory
	};

	enum class eMeshType
	{
		None,
		Static,
		Skinned
	};

	struct StaticVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texCoords;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		
		StaticVertex() = default;
		StaticVertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& tex, const DirectX::XMFLOAT3& norm, const DirectX::XMFLOAT3& tan)
			: position(pos), texCoords(tex), normal(norm), tangent(tan)
		{}
	};

	class StaticMesh : public Resource
	{
	public:
		StaticMesh();
		~StaticMesh() override;

		virtual bool CreateBuffers();
		virtual void Clear();

		void Draw(ID3D11DeviceContext* deviceContext);

		void ComputeBouingBox();
		
		void AddVertices(const std::vector<StaticVertex>& vertices, uint32_t* outOffset = nullptr);
		void AddIndices(const std::vector<uint32_t>& indices, uint32_t* outOffset = nullptr);

		std::vector<StaticVertex> GetVertices() { return m_Vertices; }
		std::vector<uint32_t> GetIndices() { return m_Indices; }

		uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_Vertices.size()); }
		uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_Indices.size()); }

		VertexBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
		IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer.get(); }

		BoundingBox* GetBoundingBox() const { return m_BoundingBox.get(); }

		Shader* GetVertexShader() const;
		std::string GetVertexShaderName() const { return m_VertexShaderName; }
		void SetVertexShaderName(const std::string& name) { m_VertexShaderName = name; }

		eSourceType GetSourceType() const { return m_SourceType; }
		void SetSourceType(eSourceType type) { m_SourceType = type; }

		std::filesystem::path GetFilepath() const { return m_Filepath; }

		eMeshType GetMeshType() const { return m_Type; }

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const { return m_PrimitiveTopology; }
		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology) { m_PrimitiveTopology; }

		static constexpr eResourceType GetType() { return eResourceType::StaticMesh; }

	protected:
		bool createIndexBuffer();

	protected:
		eSourceType m_SourceType = eSourceType::None;
		std::filesystem::path m_Filepath;	// 좀 애매해다. 

		eMeshType m_Type = eMeshType::None;

		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		std::vector<StaticVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		
		std::unique_ptr<BoundingBox> m_BoundingBox;

		// 이것두 Material이 관리하도록 할 거다.
		std::string m_VertexShaderName = "Default_VS";
	};
}
