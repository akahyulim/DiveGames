#pragma once
#include "GraphicsInclude.h"
#include <memory>

namespace dive
{
	// texture 생성 테스트용
	// => 이게 맞는지 정말 모르겠다.
	namespace graphics
	{
		// Texture1D, 2D, 3D의 Desc가 다르기 때문에 하나로 만드는게 낫다.
		struct TextureDesc
		{
			enum eTextureType
			{
				Texture1D,
				Texture2D,
				Texture3D,
			} type = Texture2D;

			unsigned int width = 0;
			unsigned int height = 0;
			DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		};

		struct GraphicsDeviceChild
		{
			std::shared_ptr<void> internalState;
			inline bool IsValid() const { return internalState.get() != nullptr; }
		};

		struct GPUResource : public GraphicsDeviceChild
		{
			enum class eGpuResourceType
			{
				Buffer,
				Texture,
				Unknown,
			} type = eGpuResourceType::Unknown;

			inline bool IsBuffer() const { return type == eGpuResourceType::Buffer; }
			inline bool IsTexture() const { return type == eGpuResourceType::Texture; }
		};

		struct Texture : public GPUResource
		{
			TextureDesc desc;

			const TextureDesc& GetDesc() const { return desc; }
		};
	}
}