#pragma once
#include "DvTexture.h"

namespace Dive
{
	class DvTexture2D : public DvTexture
	{
		DV_CLASS(DvTexture2D, DvTexture)

	public:
		DvTexture2D();
		DvTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool mipChain = true);
		DvTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t mipCount);
		DvTexture2D(uint32_t width, uint32_t height, uint32_t depth, bool readOnly = false);
		// 유니티에서는 개별 객체에서 직접 로드하는 함수를 제공하지 않는다.
		// 대신 리소스 매니저에서 파일을 전달받아 객체를 생성한다.
		// => 실제로는 LoadImage라는 메서드로 로드된 데이터를 전달받아 생성할 수 있다.
		// => 즉, 아래처럼 생성자를 통해 로드하는 게 잘못된 구현이다.
		DvTexture2D(const std::string& filename, bool mipChain = true);
		DvTexture2D(const std::string& filename, uint32_t size, const void* pSource, bool mipChain = true);
		virtual ~DvTexture2D() = default;

		// 유니티의 LoadImage에 대응해야 한다.
		bool LoadFromFile(const std::string& filename) override;
		bool LoadFromMemory(const std::string& filename, size_t size, const void* pSource);
		bool UpdateSubresource(const void* pData, uint32_t size);

	private:
		bool createColorBuffer();
		bool createDepthBuffer(bool readOnly);
		
	private:
		DXGI_FORMAT m_Format;
	};
}