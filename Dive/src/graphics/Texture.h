#pragma once
#include "resource/Resource.h"

namespace Dive
{
	enum class eShaderResourceSlot : uint8_t
	{
		// 🔹 머티리얼 텍스처 슬롯 (Geometry Pass)
		Diffuse = 0,
		NormalMap = 1,
		RoughnessMap = 2,
		MetallicMap = 3,
		EmissiveMap = 4,
		OpacityMap = 5,

		// 🔸 G-Buffer 결과물 슬롯 (Lighting Pass)
		GBuffer_Albedo = 10,
		GBuffer_Normal = 11,
		GBuffer_RoughMetal = 12,
		GBuffer_Emissive = 13,
		GBuffer_Depth = 14,
		GBuffer_MaterialID = 15,

		// 🔸 조명/쉐도우/환경맵
		ShadowMap = 20,
		SSAOMap = 21,
		EnvironmentMap = 22,
		IBL_Irradiance = 23,
		IBL_Specular = 24,
		BRDF_LUT = 25,

		// 🔸 후처리용
		SceneColor = 30,
		SceneDepth = 31,
		BloomTexture = 32,
		MotionVector = 33,

		// ✅ 마지막 슬롯 수 확인용
		Count
	};

	class Texture : public Resource
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;

		virtual bool Create() = 0;
		virtual void Release();

		void Bind(eShaderResourceSlot slot);

		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }

		DXGI_FORMAT GetFormat() const { return m_format; }
		bool IsGenerateMips() const { return m_useMips; }
		uint32_t GetMipLevels() const { return m_mipLevels; }

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }

		static uint32_t CalculateMipmapLevels(uint32_t width, uint32_t height);
		static uint32_t GetPixelSize(DXGI_FORMAT format);
		static bool CanGenerateMips(DXGI_FORMAT format);

	protected:
		uint32_t m_width = 1;
		uint32_t m_height = 1;

		DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;
		
		uint32_t m_mipLevels = 1;
		bool m_useMips = false;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	};
}
