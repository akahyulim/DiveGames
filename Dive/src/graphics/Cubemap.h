#pragma once
#include "Texture.h"

namespace Dive
{
	class Cubemap : public Texture
	{
	public:
		Cubemap() = delete;
		Cubemap(UINT32 size, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMips = false);
		~Cubemap() override;

		bool Create() override;
		void Release() override;

		UINT32 GetSize() const { return m_Size; }

		void SetFaceData(UINT32 index, const void* pixels, size_t size);

		ID3D11RenderTargetView* GetBackBufferRTV() const { return m_RenderTargetView; }

		static constexpr eResourceType GetType() { return eResourceType::Cubemap; }

		static std::shared_ptr<Cubemap> LoadFromFaceFiles(const std::vector<std::filesystem::path>& faceFilepaths, bool useMips = false);
		static std::shared_ptr<Cubemap> LoadFromFile(const std::filesystem::path& filepath, bool useMips = false);

	private:
		UINT32 m_Size = 0;

		ID3D11RenderTargetView* m_RenderTargetView = nullptr;
		std::array<std::vector<uint8_t>, 6> m_FaceData;
	};
}
