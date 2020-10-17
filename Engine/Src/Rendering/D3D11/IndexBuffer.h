#pragma once
#include "Core/Object.h"

namespace Dive
{
	class RenderDevice;

	class IndexBuffer : public Object
	{
		IndexBuffer(RenderDevice* device);
		~IndexBuffer();

		bool Create(const std::vector<unsigned int>& indices);
		bool CreateDynamic(unsigned int size);

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer() const { return m_buffer.get(); }

	private:
		RenderDevice* m_device;
		std::shared_ptr<ID3D11Buffer> m_buffer;
	};
}

