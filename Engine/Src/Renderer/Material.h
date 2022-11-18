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

	// GraphicsDef�� �ű��
	enum eTextureUnit
	{
		Diffuse = 0,
		Normal = 1,
		ShadowMap = 3,
		Max = 4
	};

	class Material : public Resource
	{
		DIVE_OBJECT(Material, Resource)

	public:
		explicit Material(Context* pContext);
		~Material() override;

	private:
	private:
		Scene* m_pScene = nullptr;

		std::unordered_map<eTextureUnit, Texture*> m_Textures;

		// ������ lod������ ������ ����ü�� ������.
		Technique* m_pTechnique = nullptr;

		// shader defines
	};
}