#pragma once
#include "GraphicsInclude.h"
#include "../../Core/Object.h"
#include <string>

namespace DiveEngine
{
	class GraphicsDevice;

	class ConstantBuffer : public Object
	{
	public:
		ConstantBuffer(GraphicsDevice* pGraphicsDevice, const std::string& name);
		~ConstantBuffer();

		template<typename T>
		bool Create(unsigned int offsetCount = 1)
		{
			m_OffsetCount = offsetCount;
			m_Stride = static_cast<uint32_t>(sizeof(T));
			m_SizeGPU = static_cast<uint64_t>(m_Stride * m_OffsetCount);

			return create();
		}

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }

	private:
		bool create();
		void destroy();

	private:
		GraphicsDevice* m_pGraphicsDevice;
		ID3D11Buffer* m_pBuffer;

		std::string m_Name;

		// 왠지 얘네는 필요없을 것 같다.
		uint32_t m_OffsetCount;
		uint32_t m_Stride;
		uint64_t m_SizeGPU;
	};
}