#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;

	class IndexBuffer : public Object
	{
		DIVE_OBJECT(IndexBuffer, Object);
		
	public:
		IndexBuffer(Context* context);
		~IndexBuffer();

		bool Create(const std::vector<unsigned int>& indices);
		bool CreateDynamic(unsigned int size);

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer() const { return m_buffer; }

	private:
		ID3D11Buffer* m_buffer = nullptr;
	};
}

