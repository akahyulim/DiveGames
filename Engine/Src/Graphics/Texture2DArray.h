#pragma once
#include "Texture.h"

namespace Dive
{
	class Texture2DArray : public Texture
	{
		DV_CLASS(Texture2DArray, Texture)

	public:
		Texture2DArray() = delete;	// 리소스 매니저가 생성할 때 필요하다.
		Texture2DArray(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool mipChain = true);
		Texture2DArray(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, uint32_t mipCount = -1);
		Texture2DArray(uint32_t width, uint32_t height, uint32_t depth, uint32_t arraySize, bool useReadOnly = false);

	private:
		bool createColorBuffer();
		bool createDepthBuffer(bool useReadOnly);

	private:
		uint32_t m_ArraySize;
	};
}