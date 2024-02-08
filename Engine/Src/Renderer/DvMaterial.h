#pragma once
#include "Graphics/GraphicsDefs.h"

namespace Dive
{
	class DvShader;
	class DvTexture;

	class DvMaterial
	{
	public:
		DvMaterial();
		DvMaterial(DvShader* pShader);
		~DvMaterial();

		 DvShader* GetShader() const { return m_pShader; }
		 void SetShader(DvShader* pShader) { m_pShader = pShader; }
		 
		 const std::unordered_map<eTextureUnit, DvTexture*>& GetTextures() const { return m_Textures; }
		 DvTexture* GetTexture(eTextureUnit unit) const;
		 void SetTexture(eTextureUnit unit, DvTexture* pTexture);
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

	public:
		static DvMaterial* Create(DvShader* pShader);

	private:
	private:
		DvShader* m_pShader;

		std::unordered_map<eTextureUnit, DvTexture*> m_Textures;
		DirectX::XMFLOAT4 m_DiffuseColor;

		DirectX::XMFLOAT2 m_Tiling;
		DirectX::XMFLOAT2 m_Offset;
	};
}