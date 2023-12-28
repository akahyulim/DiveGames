#include "DivePch.h"
#include "Material.h"
#include "Graphics/Texture2D.h"
#include "Resource/ResourceCache.h"
#include "IO/FileStream.h"
#include "Core/Log.h"

namespace Dive
{
	Material::Material()
		: Resource(eResourceType::Material),
        m_DiffuseColor(1.0f, 0.0f, 0.0f, 1.0f),
        m_Tiling(1.0f, 1.0f),
        m_Offset(0.0f, 0.0f)
	{
        // sparky는 shader를 가진다.
        // 따라서 이 곳에서 shader를 통해 constantbuffer의 userbuffer 정보(크기, 타입)을 참조하여 버퍼를 직접 구성한다.
        // 그리고 bind에서 shader에 해당 버퍼를 건내주면 복사(map/unmap)를 한 후 bind한다. 
	}
		
	Material::~Material()
	{
	}

	bool Material::LoadFromFile(const std::string& filePath)
	{
		return true;
	}

	bool Material::SaveToFile(const std::string& filePath)
	{
		DV_CORE_DEBUG("Material SaveToFile: {:s}", filePath);
		
		return true;
	}

    void Material::Bind()
    {
        // 쉐이더 바인드

        // 쉐이더에 상수 버퍼 전달 

        // 텍스쳐 바인드
    }

    void Material::Unbind()
    {
        // 사용한 텍스쳐들을 언바인드
        // 그런데 스파키의 경우 텍스쳐조차 바인드/언바인드가 있다.
    }

    Texture* Material::GetTexture(eTextureUnit unit) const
    {
        auto it = m_Textures.find(unit);

        return it != m_Textures.end() ? it->second : nullptr;
    }

    void Material::SetTexture(eTextureUnit unit, Texture* pTexture)
    {
        if (unit < eTextureUnit::Max_Num)
        {
            if (pTexture)
                m_Textures[unit] = pTexture;
            else
                m_Textures.erase(unit); // 이게 맞나 모르겠다.
        }
    }

    // urho는 load에서만 텍스쳐를 로드하기에 직접 만들어봤다.
    // 그런데 cache에서 GetResource한 후 위의 SetTexture하는게 맞지 않나 싶다.
    // 하지만 이 구현이 힘든게 Cache의 GetResource는 일반 리소스 파일과 엔진의 설정 파일을
    // 구분하여 로드할 수 있다는 것이다.
    void Material::AddTexture(eTextureUnit unit, const std::string& name)
    {
        if (unit < eTextureUnit::Max_Num)
        {
            // 원래는 타입에 맞춰서 시도해야 한다.
            Texture2D* pTexture = ResourceCache::GetResourceByPath<Texture2D>(name);    // 원래 ByName으로 했는데, 캐시에서 찾는거라 적용이 안될 수 있었다.
            SetTexture(unit, dynamic_cast<Texture*>(pTexture));
        }
    }

    bool Material::HasTexture(eTextureUnit unit)
    {
        return m_Textures[unit] != nullptr;
    }
}