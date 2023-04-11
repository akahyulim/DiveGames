#pragma once
#include "Resource/Resource.h"
#include "Graphics/GraphicsDefs.h"

namespace Dive
{
	class Texture;

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

		DirectX::XMFLOAT4 GetColorAlbedo() const { return m_Albedo; }
		void SetColorAlbedo(float r, float g, float b, float a) { SetColorAlbedo(DirectX::XMFLOAT4(r, g, b, a)); }
		void SetColorAlbedo(const DirectX::XMFLOAT4& color) { m_Albedo = color; }

	private:
	private:
		std::unordered_map<eTextureUnit, Texture*> m_Textures;
		DirectX::XMFLOAT4 m_Albedo;
	};
}