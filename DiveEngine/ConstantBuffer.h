#pragma once
#include "GraphicsInclude.h"
#include "Object.h"
#include <string>

namespace dive
{
	class GraphicsDevice;

	class ConstantBuffer : public Object
	{
	public:
		ConstantBuffer(GraphicsDevice* graphicsDevice, const std::string& name);
		~ConstantBuffer();

		template<typename T>
		bool Create(unsigned int offsetCount = 1)
		{
			mOffsetCount = offsetCount;
			mStride = static_cast<uint32_t>(sizeof(T));
			mSizeGPU = static_cast<uint64_t>(mStride * mOffsetCount);

			return create();
		}

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer() const { return mBuffer; }

	private:
		bool create();
		void destroy();

	private:
		GraphicsDevice* mGraphicsDevice;
		ID3D11Buffer* mBuffer;

		std::string mName;

		// 왠지 얘네는 필요없을 것 같다.
		uint32_t mOffsetCount;
		uint32_t mStride;
		uint64_t mSizeGPU;
	};
}