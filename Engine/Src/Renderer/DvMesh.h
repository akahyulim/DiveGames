#pragma once

namespace Dive
{
	class DvVertexBuffer;
	class DvIndexBuffer;

	// urho3d의 Geometry
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

		// primitive topology도 받는다.
		//bool SetDrawRange(unsigned int indexStart, unsigned int indexCount);
		
		// draw까지 여기에 있다.
		// graphics를 매개변수로 받은 후 set vertex, index buffer 후 graphics의 draw에 primitive, start, count 등을 전달한다.

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