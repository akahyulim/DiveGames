#pragma once
#include "Texture.h"

namespace Dive
{
	class Texture2D : public Texture
	{
		DV_CLASS(Texture2D, Texture)

	public:
		Texture2D() = delete;
		Texture2D(UINT32 width, UINT32 height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMips = false);
		~Texture2D() override;

		bool Create() override;

		UINT32 GetWidth() const { return m_Width; }
		UINT32 GetHeight() const { return m_Height; }

		void SetPixelData(const void* pixels, size_t size);

		static std::shared_ptr<Texture2D> LoadFromFile(const std::filesystem::path& filepath, bool useMips = false);
		static std::shared_ptr<Texture2D> LoadFromMemory(const std::filesystem::path& filepath, size_t size, const void* pSource, bool useMips = false);

	private:
		UINT32 m_Width = 0;
		UINT32 m_Height = 0;
		std::vector<uint8_t> m_PixelData;
	};
}
