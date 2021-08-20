#pragma once
#include "GraphicsInclude.h"
#include <memory>

namespace dive
{
	// texture ���� �׽�Ʈ��
	// => �̰� �´��� ���� �𸣰ڴ�.
	namespace graphics
	{
		// Texture1D, 2D, 3D�� Desc�� �ٸ��� ������ �ϳ��� ����°� ����.
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