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

		ID3D11Buffer* GetBuffer() const { return m_Buffer; }

	private:
		RenderDevice* m_Device = nullptr;
		ID3D11Buffer* m_Buffer = nullptr;
	};
}

