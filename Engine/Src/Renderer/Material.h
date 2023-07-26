#pragma once
#include "Resource/Resource.h"
#include "Graphics/GraphicsDefs.h"

namespace Dive
{
	class Texture;

	// 유니티는 Material에서 Shader를 선택할 수 있으며,
	// 기본적으로 Standard(pbs 포함)와 Legacy 두 빌트인 셰이더를 제공한다.
	class Material : public Resource
	{
	public:
		Material();
		~Material() override;

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		const std::unordered_map<eTextureUnit, Texture*>& GetTextures() const { return m_Textures; }
		Texture* GetTexture(eTextureUnit unit) const;
		void SetTexture(eTextureUnit unit, Texture* pTexture);
		void AddTexture(eTextureUnit unit, const std::string& name);

		// albeo
		DirectX::XMFLOAT4 GetAlbedoColor() const { return m_AlbedoColor; }
		void SetAlbedoColor(float r, float g, float b, float a) { SetAlbedoColor(DirectX::XMFLOAT4(r, g, b, a)); }
		void SetAlbedoColor(const DirectX::XMFLOAT4& color) { m_AlbedoColor = color; }

		// metalic
		// slide bar, smoothness?, source?

		DirectX::XMFLOAT2 GetTiling() const { return m_Tiling; }
		void SetTiling(DirectX::XMFLOAT2 tiling) { m_Tiling = tiling; }
		void SetTiling(float x, float y) { m_Tiling.x = x; m_Tiling.y = y; }

		DirectX::XMFLOAT2 GetOffset() const { return m_Offset; }
		void SetOffset(DirectX::XMFLOAT2 offset) { m_Offset = offset; }
		void SetOffset(float x, float y) { m_Offset.x = x; m_Offset.y = y; }

	private:
	private:
		std::unordered_map<eTextureUnit, Texture*> m_Textures;
		DirectX::XMFLOAT4 m_AlbedoColor;

		DirectX::XMFLOAT2 m_Tiling;
		DirectX::XMFLOAT2 m_Offset;
		
	};
}