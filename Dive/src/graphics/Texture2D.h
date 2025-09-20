#pragma once
#include "Texture.h"

namespace Dive
{
	class Texture2D : public Texture
	{
	public:
		Texture2D();
		Texture2D(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMips = false);
		~Texture2D() override;

		bool Create() override;

		void SetPixelData(const void* pixels, size_t size);

		bool LoadFromFile(const std::filesystem::path& filepath) override;

		bool LoadFromMemory(const std::filesystem::path& filepath, const void* sourceData, size_t size, bool useMips = false);

		eResourceType GetType() override { return eResourceType::Texture2D; }
		static constexpr eResourceType StaticType() { return eResourceType::Texture2D; }

	private:
		std::vector<uint8_t> m_pixelData;
	};
}
