#pragma once
#include "GraphicsDevice.h"
#include <vector>


namespace Dive
{
	enum class eBufferType
	{
		Invalid,
		VertexBuffer,
		IndexBuffer,
		ConstantBuffer
	};

	class Buffer
	{
	public:
		Buffer() = default;
		~Buffer() = default;

		template<typename T>
		bool CreateBuffer(eBufferType type, const std::vector<T>& datas)
		{

			m_type = type;
			//m_bDynamic = dynamic;
			m_stride = static_cast<unsigned int>(sizeof(T));
			//m_count = dynamic ? 1 : datas->size();

			return genenrateBuffer(static_cast<const void*>(datas.data()));
		}

		template<typename T>
		bool CreateDynamicBuffer(eBufferType)
		{
			return generateBuffer(nullptr);
		}

		void* Map();
		void Unmap();

	private:
		bool generateBuffer(const void* datas);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		bool m_bDynamic = false;
		unsigned int m_stride = 0;
		unsigned int m_count = 0;

		eBufferType m_type = eBufferType::Invalid;
	};
}