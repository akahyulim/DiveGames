#pragma once
#include "resource/Resource.h"
#include "graphics/Graphics.h"

namespace Dive
{
	class Texture;
	class Texture2D;
	class Shader;
	class ShaderProgram;
	class ConstantBuffer;

	enum class eTextureUnitType : uint8_t
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
		Material(ID3D11Device* device);
		~Material() = default;

		bool LoadFromFile(const std::filesystem::path& filepath) override;
		bool SaveToFile(const std::filesystem::path& filepath) override;

		void Bind(ID3D11DeviceContext* deviceContext);

		std::shared_ptr<Texture2D> GetTexture(eTextureUnitType type);
		void SetTexture(eTextureUnitType type, std::shared_ptr<Texture2D> texture);
		void SetTexture(eTextureUnitType type, const std::string& textureName);

		DirectX::XMFLOAT4 GetDiffuseColor() const { return m_cpuBuffer.diffuseColor; }
		void SetDiffuseColor(const DirectX::XMFLOAT4& color) { m_cpuBuffer.diffuseColor = color; m_isDirty = true; }
		void SetDiffuseColor(float r, float g, float b, float a) { m_cpuBuffer.diffuseColor = { r, g, b, a };  m_isDirty = true; }

		DirectX::XMFLOAT2 GetTiling() const { return m_cpuBuffer.tiling; }
		void SetTiling(float x, float y) { m_cpuBuffer.tiling = { x, y };  m_isDirty = true; }

		DirectX::XMFLOAT2 GetOffset() const { return m_cpuBuffer.offset; }
		void SetOffset(float x, float y) { m_cpuBuffer.offset = { x, y };  m_isDirty = true; }

		void SetShaders(ShaderProgram* shaders) { m_shaders = shaders; }

		eBlendMode GetBlendMode() const { return m_blendMode; }
		void SetBlendMode(eBlendMode mode) { m_blendMode = mode; }

		bool IsTransparent() const;

		static constexpr eResourceType GetType() { return eResourceType::Material; }

	private:
		std::unordered_map<uint8_t, std::shared_ptr<Texture2D>> m_textures;

		bool m_isDirty = true;

		MaterialConstants m_cpuBuffer;
		std::unique_ptr<ConstantBuffer> m_gpuBuffer;

		ShaderProgram* m_shaders = nullptr;

		eBlendMode m_blendMode = eBlendMode::Opqaue;
	};
}
