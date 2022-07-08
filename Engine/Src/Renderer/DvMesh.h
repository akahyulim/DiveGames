#pragma once

namespace Dive
{
	class DvVertexBuffer;
	class DvIndexBuffer;

	// urho3d�� Geometry
	class DvMesh
	{
	public:
		DvMesh();

		DvVertexBuffer* GetVertexBuffer(unsigned int index) const;
		void SetVertexBuffer(unsigned int index, DvVertexBuffer* pBuffer);
		unsigned int GetVertexBufferCount() const { return static_cast<unsigned int>(m_VertexBuffers.size()); }
		void SetNumVertexBuffers(unsigned int num);

		DvIndexBuffer* GetIndexBuffer() { return m_pIndexBuffer; }
		void SetIndexBuffer(DvIndexBuffer* pBuffer) { m_pIndexBuffer = pBuffer; }

		// primitive topology�� �޴´�.
		//bool SetDrawRange(unsigned int indexStart, unsigned int indexCount);
		
		// draw���� ���⿡ �ִ�.
		// graphics�� �Ű������� ���� �� set vertex, index buffer �� graphics�� draw�� primitive, start, count ���� �����Ѵ�.

	private:
	private:
		std::vector<DvVertexBuffer*> m_VertexBuffers;
		DvIndexBuffer* m_pIndexBuffer = nullptr;

		unsigned int m_VertexStart = 0;
		unsigned int m_VertexCount = 0;
		unsigned int m_IndexStart = 0;
		unsigned int m_IndexCount = 0;

		// vetex element

		// vertex, index raw data

	};
}