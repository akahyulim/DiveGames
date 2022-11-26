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

	// GraphicsDef로 옮기기
	enum eTextureUnit
	{
		Diffuse = 0,
		Normal = 1,
		ShadowMap = 3,
		Max = 4
	};

	// 일단 텍스쳐 부분은 제외하고 구현
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

		// 원래는 lod때문에 랩핑한 구조체를 가진다.
		Technique* m_pTechnique = nullptr;

		// ShaderDefines와 Parameter를 관리하지만 일단 뒤로 미룬다.
	};
}