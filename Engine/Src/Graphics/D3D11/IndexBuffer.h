#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class RenderDevice;

	class IndexBuffer : public Object
	{
		DIVE_OBJECT(IndexBuffer, Object);
		
	public:
		IndexBuffer(Context* context, const std::shared_ptr<RenderDevice>& device);
		~IndexBuffer();

		bool Create(const std::vector<unsigned int>& indices);
		bool CreateDynamic(unsigned int size);

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer() const { return m_buffer; }

	private:
		std::shared_ptr<RenderDevice> m_renderDevice;
		ID3D11Buffer* m_buffer = nullptr;
	};
}

