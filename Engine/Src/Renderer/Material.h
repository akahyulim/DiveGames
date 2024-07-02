#pragma once
#include "Graphics/Graphics.h"
#include "Resource/Resource.h"

namespace Dive
{
	class Texture;

	// ���� Texture�� opaque ���θ� �Ǵ����� �ʴ´�.
	// �̴� ����Ƽ�� ������ ���� ������, ����Ƽ�� ��� ��Ƽ���󿡼� Ȯ���Ѵ�.
	// ����� legacy��. ���� pbs�� �߰��� �� �ִ�.
	class Material : public Resource
	{
		DV_CLASS(Material, Resource);

	public:
		Material();
		~Material() override;

		bool LoadFromFile(const std::string& fileName) override;
		bool SaveToFile(const std::string& fileName) override;

		const std::unordered_map<eTextureUnitType, Texture*>& GetTextures() const { return m_Textures; }
		Texture* GetTexture(eTextureUnitType unit) const;
		void SetTexture(eTextureUnitType unit, Texture* pTexture);
		void AddTexture(eTextureUnitType unit, const std::string& name);
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

	private:
		std::unordered_map<eTextureUnitType, Texture*> m_Textures;
		
		DirectX::XMFLOAT4 m_DiffuseColor;
		DirectX::XMFLOAT2 m_Tiling;
		DirectX::XMFLOAT2 m_Offset;
	};

	Material* LoadMaterialFromFile(const std::string& fileName);
	Material* CreateMaterial(const std::string& name);
}