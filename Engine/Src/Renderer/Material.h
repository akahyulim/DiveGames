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
	enum eTextureUnit
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

		Technique* GetTechnique() const { return m_pTechnique; }
		void SetTechnique(Technique* pTech);
		
		// overide
		bool Load(FileStream* pDeserializer) override;

		// static
		static void RegisterObject(Context* pContext);

	private:
	private:
		Scene* m_pScene = nullptr;

		std::unordered_map<eTextureUnit, Texture*> m_Textures;

		// ������ lod������ ������ ����ü�� ������.
		Technique* m_pTechnique = nullptr;

		// ShaderDefines�� Parameter�� ���������� �ϴ� �ڷ� �̷��.
	};
}