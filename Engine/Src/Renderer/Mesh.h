#pragma once
#include "Graphics/GraphicsDefs.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"

namespace Dive
{
	class IMesh
	{
	public:
		IMesh();
		virtual ~IMesh();

		virtual uint32_t GetNumVertices() const = 0;
		virtual uint32_t GetNumIndices() const = 0;

		std::string GetName() const { return m_Name; }
		void SetName(std::string name) { m_Name = name; }

		std::string GetNodeName() const { return m_NodeName; }
		void SetNodeName(std::string nodeName) { m_NodeName = nodeName; }

		virtual void CreateVertexBuffer() = 0;
		virtual void CreateIndexBuffer() = 0;

		VertexBuffer* GetVertexBuffer() const { return m_pVertexBuffer; }
		IndexBuffer* GetIndexBuffer() const { return m_pIndexBuffer; }

	protected:
		std::string m_Name;
		std::string m_NodeName;

		VertexBuffer* m_pVertexBuffer;
		IndexBuffer* m_pIndexBuffer;
	};

	class DvStaticMesh : public IMesh
	{
	public:
		DvStaticMesh() = default;
		DvStaticMesh(const std::string& name, std::vector<VertexStatic>& vertices, std::vector<uint32_t>& indices);
		DvStaticMesh(const std::string& nodeName, const std::string& name, std::vector<VertexStatic>& vertices, std::vector<uint32_t>& indices);
		virtual ~DvStaticMesh();

		void CreateVertexBuffer() override;
		void CreateIndexBuffer() override;

		uint32_t GetNumVertices() const override { return static_cast<uint32_t>(m_Vertices.size()); }
		uint32_t GetNumIndices() const override { return static_cast<uint32_t>(m_Indices.size()); }

	private:
	private:
		std::vector<VertexStatic> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}