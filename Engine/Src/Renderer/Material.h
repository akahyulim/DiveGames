#pragma once
#include "Graphics/GraphicsDefs.h"
#include "Resource/Resource.h"

namespace Dive
{
	class Shader;
	class Texture;

	// 현재는 legacy다. 추후 pbs가 추가될 수 있다.
	class Material : public Resource
	{
		DV_OBJECT(Material, Resource);

	public:
		Material();
		~Material() override;

		bool LoadFromFile(const std::string& fileName) override;
		bool SaveToFile(const std::string& fileName) override;

		 Shader* GetShader() const { return m_pShader; }
		 void SetShader(Shader* pShader) { m_pShader = pShader; }
		 
		 const std::unordered_map<eTextureUnit, Texture*>& GetTextures() const { return m_Textures; }
		 Texture* GetTexture(eTextureUnit unit) const;
		 void SetTexture(eTextureUnit unit, Texture* pTexture);
		 void AddTexture(eTextureUnit unit, const std::string& name);
		 bool HasTexture(eTextureUnit unit);

		 DirectX::XMFLOAT4 GetDiffuseColor() const { return m_DiffuseColor; }
		 void SetDiffuseColor(float r, float g, float b, float a) { m_DiffuseColor = { r, g, b, a }; }
		 void SetDiffuseColor(const DirectX::XMFLOAT4& color) { m_DiffuseColor = color; }

		 DirectX::XMFLOAT2 GetTiling() const { return m_Tiling; }
		 void SetTiling(DirectX::XMFLOAT2 tiling) { m_Tiling = tiling; }
		 void SetTiling(float x, float y) { m_Tiling.x = x; m_Tiling.y = y; }

		 DirectX::XMFLOAT2 GetOffset() const { return m_Offset; }
		 void SetOffset(DirectX::XMFLOAT2 offset) { m_Offset = offset; }
		 void SetOffset(float x, float y) { m_Offset.x = x; m_Offset.y = y; }

	private:
		Shader* m_pShader;
		std::unordered_map<eTextureUnit, Texture*> m_Textures;
		DirectX::XMFLOAT4 m_DiffuseColor;
		DirectX::XMFLOAT2 m_Tiling;
		DirectX::XMFLOAT2 m_Offset;
	};

	Material* LoadMaterialFromFile(const std::string& fileName);
	Material* CreateMaterial(const std::string& name);
}