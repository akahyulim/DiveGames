#pragma once
#include "GraphicsDevice.h"
#include <vector>

namespace Dive
{
	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		~IndexBuffer() = default;

		template<typename T>
		bool Create(const std::vector<T>& indices)
		{
			m_bDynamic = false;
			m_stride = static_cast<unsigned int>(sizeof(T));
			m_indexCount = static_cast<unsigned int >(indices.size());

			return createBuffer(static_cast<const void*>(indices.data()));
		}

		template<typename T>
		bool CreateDynamic(unsigned int size)
		{
			m_bDynamic = true;
			m_stride = static_cast<unsigned int>(sizeof(T));
			m_indexCount = size;

			return createBuffer(nullptr);
		}

		void* Map();
		void Unmap();

	private:
		bool createBuffer(const void* datas);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		bool m_bDynamic = false;
		unsigned int m_stride = 0;
		unsigned int m_indexCount = 0;
	};
}