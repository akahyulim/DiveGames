#pragma once
#include "Core/Object.h"
#include "Core/Context.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"
#include "Graphics/Graphics.h"

namespace Dive
{
	class Context;

	class IndexBuffer : public Object
	{
		DIVE_OBJECT(IndexBuffer, Object);

	public:
		IndexBuffer(Context* context);
		~IndexBuffer();

		template<typename T = unsigned int>
		bool Create(const std::vector<T>& indices);
		template<typename T = unsigned int>
		bool CreateDynamic(unsigned int size);

		void* Map();
		bool Unmap();

		unsigned int GetCount() const	{ return m_count; }
		ID3D11Buffer* GetBuffer() const	{ return m_buffer; }
		DXGI_FORMAT GetFormat() const;

	private:
		bool createBuffer(const void* indices);

	private:
		ID3D11Buffer* m_buffer;
		
		bool m_bDynamic;
		unsigned int m_stride;
		unsigned int m_count;
	};

	template<typename T>
	inline bool IndexBuffer::Create(const std::vector<T>& indices)
	{
		m_bDynamic	= false;
		m_stride	= sizeof(T);
		m_count		= static_cast<unsigned int>(indices.size());

		return createBuffer(indices.data());
	}

	template<typename T>
	inline bool IndexBuffer::CreateDynamic(unsigned int size)
	{
		m_bDynamic	= true;
		m_stride	= sizeof(T);
		m_count		= size;

		return createBuffer(nullptr);
	}
}

