#pragma once
#include "resource/Resource.h"
#include "graphics/Graphics.h"
#include "graphics/Shader.h"

namespace Dive
{
	class Texture;
	class Texture2D;

	enum class eTextureUnitType
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

	class Material : public Resource
	{
		DV_CLASS(Material, Resource)

	public:
		Material();
		~Material() = default;

		bool LoadFromFile(const std::filesystem::path& filepath) override;
		bool SaveToFile(const std::filesystem::path& filepath) override;

		const std::unordered_map<eTextureUnitType, Texture2D*>& GetTextures() const { return m_Textures; }
		Texture2D* GetTexture(eTextureUnitType unit) const;
		void SetTexture(eTextureUnitType unit, Texture2D* texture);
		bool HasTexture(eTextureUnitType unit) const;

		DirectX::XMFLOAT4 GetDiffuseColor() const { return m_DiffuseColor; }
		void SetDiffuseColor(float r, float g, float b, float a) { m_DiffuseColor = { r, g, b, a }; }
		void SetDiffuseColor(const DirectX::XMFLOAT4& color) { m_DiffuseColor = color; }

		DirectX::XMFLOAT2 GetTiling() const { return m_Tiling; }
		void SetTiling(DirectX::XMFLOAT2 tiling) { m_Tiling = tiling; }
		void SetTiling(float x, float y) { m_Tiling.x = x; m_Tiling.y = y; }

		DirectX::XMFLOAT2 GetOffset() const { return m_Offset; }
		void SetOffset(DirectX::XMFLOAT2 offset) { m_Offset = offset; }
		void SetOffset(float x, float y) { m_Offset.x = x; m_Offset.y = y; }

		bool IsOpaque() const;

		Shader* GetPixelShader() const { return m_PixelShader; }
		void SetPixelShader(Shader* shader)
		{
			if (m_PixelShader != shader)
			{
				m_PixelShader = shader;
			}
		}

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
		std::string m_Name{};

		std::unordered_map<eTextureUnitType, Texture2D*> m_Textures;
		
		DirectX::XMFLOAT4 m_DiffuseColor{ 1.0f, 1.0f, 1.0f, 1.0f };		
		DirectX::XMFLOAT2 m_Tiling{ 1.0f, 1.0f };
		DirectX::XMFLOAT2 m_Offset{ 0.0f, 0.0f };

		Shader* m_PixelShader = nullptr;
	};
}