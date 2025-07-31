#pragma once
#include "Texture.h"

namespace Dive
{
	class Cubemap : public Texture
	{
	public:
		Cubemap() = delete;
		Cubemap(uint32_t size, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMips = false);
		~Cubemap() override;

		bool Create() override;
		void Release() override;

		uint32_t GetSize() const { return m_Size; }

		void SetFaceData(uint32_t index, const void* pixels, size_t size);

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView.Get(); }

		static constexpr eResourceType GetType() { return eResourceType::Cubemap; }

		static std::shared_ptr<Cubemap> LoadFromFaceFiles(const std::vector<std::filesystem::path>& faceFilepaths, bool useMips = false);
		static std::shared_ptr<Cubemap> LoadFromFile(const std::filesystem::path& filepath, bool useMips = false);

	private:
		uint32_t m_Size = 0;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		std::array<std::vector<uint8_t>, 6> m_FaceData;
	};
}
