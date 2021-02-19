#pragma once
#include "Resource/Resource.h"
#include "Core/DiveDefs.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/D3D11/VertexBuffer.h"
#include "Graphics/D3D11/IndexBuffer.h"


namespace Dive
{
	enum class eBufferType
	{
		VertexBuffer,
		IndexBuffer
	};

	// �ᱹ Command���� Data�� ���ε�...
	// draw �Լ��� �� ���� ����� �� ���?
	// => �������̽� ���ϼ��� �����ϴ� ���� ���� �� ����.
	class Mesh : public Resource
	{
	public:
		Mesh(Context* context);
		virtual ~Mesh();

		// save & load

		template<typename T>
		bool CreateBuffer(eBufferType type, const std::vector<T>& data);
		
		// get & set
		VertexBuffer* GetVertexBuffer() const	{ return m_vertexBuffer; }
		IndexBuffer* GetIndexBuffer() const		{ return m_indexBuffer; }

		eVertexType GetVertexType() const 
		{
			if (m_vertexBuffer)
				return m_vertexBuffer->GetVertexType();

			return eVertexType::Unknown;
		}

	private:
		Mesh(const Mesh&)				= delete;
		Mesh& operator=(const Mesh&)	= delete;

	private:
		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;
	};

	template<typename T>
	inline bool Mesh::CreateBuffer(eBufferType type, const std::vector<T>& data)
	{
		if (data.empty())
		{
			CORE_ERROR("");
			return false;
		}

		if (type == eBufferType::VertexBuffer)
		{
			SAFE_DELETE(m_vertexBuffer);

			m_vertexBuffer = new VertexBuffer(m_context);
			return m_vertexBuffer->Create<T>(data);
		}
		else
		{
			SAFE_DELETE(m_indexBuffer);

			m_indexBuffer = new IndexBuffer(m_context);
			return m_indexBuffer->Create<T>(data);
		}

		return false;
	}
}