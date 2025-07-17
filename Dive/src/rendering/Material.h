#pragma once
#include "resource/Resource.h"
#include "graphics/Graphics.h"

namespace Dive
{
	class Texture;
	class Texture2D;
	class Shader;
	class ConstantBuffer;
	class DvConstantBuffer;

	enum class eTextureUnitType : UINT8
	{
		None = 0,
		Diffuse,
		Normal,
		Specular,
		Emissive,
		Roughness,
		MRAO, // Metalness, Roughness, AO
		Count
	};

	enum class eBlendMode
	{
		Opqaue,
		AlphaBlend,
		Additive,
		Multipy
	};

	struct MaterialConstants
	{
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT2 tiling;
		DirectX::XMFLOAT2 offset;
	};

	class Material : public Resource
	{
	public:
		Material();
		~Material() = default;

		bool LoadFromFile(const std::filesystem::path& filepath) override;
		bool SaveToFile(const std::filesystem::path& filepath) override;

		void Bind(ID3D11DeviceContext* deviceContext);

		std::shared_ptr<Texture2D> GetTexture(eTextureUnitType type);
		void SetTexture(eTextureUnitType type, std::shared_ptr<Texture2D> texture);
		void SetTexture(eTextureUnitType type, const std::string& textureName);

		DirectX::XMFLOAT4 GetDiffuseColor() const { return m_CpuBuffer.diffuseColor; }
		void SetDiffuseColor(const DirectX::XMFLOAT4& color) { m_CpuBuffer.diffuseColor = color; m_Dirty = true; }
		void SetDiffuseColor(float r, float g, float b, float a) { m_CpuBuffer.diffuseColor = { r, g, b, a };  m_Dirty = true; }

		DirectX::XMFLOAT2 GetTiling() const { return m_CpuBuffer.tiling; }
		void SetTiling(float x, float y) { m_CpuBuffer.tiling = { x, y };  m_Dirty = true; }

		DirectX::XMFLOAT2 GetOffset() const { return m_CpuBuffer.offset; }
		void SetOffset(float x, float y) { m_CpuBuffer.offset = { x, y };  m_Dirty = true; }

		Shader* GetPixelShader() const;
		std::string GetPixelShaderName() const { return m_PixelShaderName; }
		void SetPixelShaderName(const std::string& name) { m_PixelShaderName = name; }

		ConstantBuffer* GetConstantBuffer();

		eBlendMode GetBlendMode() const { return m_BlendMode; }
		void SetBlendMode(eBlendMode mode) { m_BlendMode = mode; }

		bool IsTransparent() const;

		static constexpr eResourceType GetType() { return eResourceType::Material; }

	private:
		std::unordered_map<UINT8, std::shared_ptr<Texture2D>> m_Textures;

		std::string m_PixelShaderName = "Default_PS";

		bool m_Dirty = true;

		MaterialConstants m_CpuBuffer;
		std::unique_ptr<ConstantBuffer> m_GpuBuffer;
		std::unique_ptr<DvConstantBuffer> m_ConstantBuffer;

		eBlendMode m_BlendMode = eBlendMode::Opqaue;
	};
}
