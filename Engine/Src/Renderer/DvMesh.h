#pragma once

namespace Dive
{
	class VertexBuffer;
	class DvIndexBuffer;

	// urho3d¿« Geometry
	class DvMesh
	{
	public:
		DvMesh();

		VertexBuffer* GetVertexBuffer(unsigned int index) const;
		void SetVertexBuffer(unsigned int index, VertexBuffer* pBuffer);
		unsigned int GetVertexBufferCount() const { return static_cast<unsigned int>(m_VertexBuffers.size()); }
		void SetNumVertexBuffers(unsigned int num);

		DvIndexBuffer* GetIndexBuffer() { return m_pIndexBuffer; }
		void SetIndexBuffer(DvIndexBuffer* pBuffer) { m_pIndexBuffer = pBuffer; }

	private:
	private:
		std::vector<VertexBuffer*> m_VertexBuffers;
		DvIndexBuffer* m_pIndexBuffer = nullptr;

		unsigned int m_VertexStart = 0;
		unsigned int m_VertexCount = 0;
		unsigned int m_IndexStart = 0;
		unsigned int m_IndexCount = 0;

		// vetex element

		// vertex, index raw data

	};
}