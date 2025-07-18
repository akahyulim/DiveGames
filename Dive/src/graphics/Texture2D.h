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

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		// 임시...
		bool IsOpaque() const { return m_Format != DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; }

		void SetPixelData(const void* pixels, size_t size);

		bool LoadFromFile(const std::filesystem::path& filepath) override;

		static std::shared_ptr<Texture2D> LoadFromMemory(const std::filesystem::path& filepath, size_t size, const void* pSource, bool useMips = false);

		static constexpr eResourceType GetType() { return eResourceType::Texture2D; }

	private:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		std::vector<uint8_t> m_PixelData;
	};
}
