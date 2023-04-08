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

	private:
	private:
		std::unordered_map<eTextureUnit, Texture*> m_Textures;
	};
}