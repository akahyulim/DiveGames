#pragma once

namespace Dive
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer() = default;
		~ConstantBuffer() { Destroy(); }

		template<typename T>
		bool Create()
		{
			m_Stride = static_cast<unsigned int>(sizeof(T));

			return createBuffer();
		}
		void Destroy();

		void* Map();
		void Unmap();

		ID3D11Buffer* GetBuffer() { return m_pBuffer; }

	private:
		bool createBuffer();

	private:
		ID3D11Buffer* m_pBuffer = nullptr;
		unsigned int m_Stride	= 0;
	};
}