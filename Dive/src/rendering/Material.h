#pragma once
#include "Graphics/Graphics.h"
#include "Resource/Resource.h"

namespace Dive
{
	class Texture;
	class Texture2D;

	// 현재 Texture는 opaque 여부를 판단하지 않는다.
	// 이는 유니티의 설정을 따른 것으로, 유니티의 경우 머티리얼에서 확인한다.
	// 현재는 legacy다. 추후 pbs가 추가될 수 있다.
	class Material : public Resource
	{
		DV_CLASS(Material, Resource);

	public:
		Material();
		~Material() override;

		bool LoadFromFile(const std::string& fileName) override;
		bool SaveToFile(const std::string& fileName) override;

		const std::unordered_map<eTextureUnitType, Texture2D*>& GetDvTextures() const { return m_DvTextures; }
		Texture2D* GetDvTexture(eTextureUnitType unit) const;
		void SetDvTexture(eTextureUnitType unit, Texture2D* pTexture);
		bool HasTexture(eTextureUnitType unit) const;
		void AddTexture(eTextureUnitType unit, const std::string& name);

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

	private:
		std::unordered_map<eTextureUnitType, Texture2D*> m_DvTextures;
		
		DirectX::XMFLOAT4 m_DiffuseColor;
		DirectX::XMFLOAT2 m_Tiling;
		DirectX::XMFLOAT2 m_Offset;
	};

	Material* LoadMaterialFromFile(const std::string& fileName);
	Material* CreateMaterial(const std::string& name);
}
