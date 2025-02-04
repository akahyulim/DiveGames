#pragma once
#include "Texture.h"

namespace Dive
{
	class Texture2D : public Texture
	{
		DV_CLASS(Texture2D, Texture)

	public:
		Texture2D();
		Texture2D(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool mipChain = true);
		Texture2D(uint32_t width, uint32_t height, DXGI_FORMAT format, uint32_t mipCount);
		Texture2D(uint32_t width, uint32_t height, uint32_t depth, bool useReadOnly = false);
		// 유니티에서는 개별 객체에서 직접 로드하는 함수를 제공하지 않는다.
		// 대신 리소스 매니저에서 파일을 전달받아 객체를 생성한다.
		// => 실제로는 LoadImage라는 메서드로 로드된 데이터를 전달받아 생성할 수 있다.
		// => 즉, 아래처럼 생성자를 통해 로드하는 게 잘못된 구현이다.
		Texture2D(const std::filesystem::path& path, bool mipChain = true);
		Texture2D(const std::filesystem::path& path, uint32_t size, const void* pSource, bool mipChain = true);
		virtual ~Texture2D() = default;

		// 유니티의 LoadImage에 대응해야 한다.
		bool LoadFromFile(const std::filesystem::path& path) override;
		bool LoadFromMemory(const std::filesystem::path& path, size_t size, const void* pSource);
		bool UpdateSubresource(const void* pData, uint32_t size);

	private:
		bool createColorBuffer();
		bool createDepthBuffer(bool readOnly);
	};
}