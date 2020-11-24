#pragma once
#include "Core/Object.h"

namespace Dive
{
	class RenderDevice;

	class IndexBuffer : public Object
	{
		IndexBuffer(const std::shared_ptr<RenderDevice>& device);
		~IndexBuffer();

		bool Create(const std::vector<unsigned int>& indices);
		bool CreateDynamic(unsigned int size);

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer() const { return m_Buffer; }

	private:
		std::shared_ptr<RenderDevice> m_Device;
		ID3D11Buffer* m_Buffer = nullptr;
	};
}

