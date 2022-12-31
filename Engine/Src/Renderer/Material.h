#pragma once
#include "Core/Object.h"
#include "Resource/Resource.h"

namespace Dive
{
	class Context;
	class Texture;
	class Texture2D;
	class Technique;
	class Scene;
	class FileStream;

	// GraphicsDef�� �ű��
	enum class eTextureUnit
	{
		Diffuse = 0,
		Normal = 1,
		ShadowMap = 3,
		Max = 4
	};

	// �ϴ� �ؽ��� �κ��� �����ϰ� ����
	class Material : public Resource
	{
		DIVE_OBJECT(Material, Resource)

	public:
		explicit Material(Context* pContext);
		~Material() override;

		// overide
		bool Load(FileStream* pDeserializer) override;

		// static
		static void RegisterObject(Context* pContext);

		Technique* GetTechnique() const { return m_pTechnique; }
		void SetTechnique(Technique* pTech);

		Texture* GetTexture(eTextureUnit unit) const;
		void SetTexture(eTextureUnit unit, Texture* pTexture);
		void AddTexture(eTextureUnit unit, const std::string& name);

		void ResetToDefault();

	private:
	private:
		Scene* m_pScene;

		std::unordered_map<eTextureUnit, Texture*> m_Textures;

		// ������ lod������ ������ ����ü�� ������.
		Technique* m_pTechnique;

		// ShaderDefines�� Parameter�� ���������� �ϴ� �ڷ� �̷��.
	};
}