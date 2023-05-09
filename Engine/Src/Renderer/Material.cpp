#include "DivePch.h"
#include "Material.h"
#include "Graphics/Texture2D.h"
#include "Resource/ResourceCache.h"
#include "IO/Log.h"

namespace Dive
{
	Material::Material()
		: Resource(eResourceType::Material)
	{
	}
		
	Material::~Material()
	{
	}

	bool Material::LoadFromFile(const std::string& filePath)
	{
		DV_CORE_DEBUG("Material LoadFromFile: {:s}", filePath);
		
		return true;
	}

	bool Material::SaveToFile(const std::string& filePath)
	{
		DV_CORE_DEBUG("Material SaveToFile: {:s}", filePath);
		
		return true;
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
            Texture2D* pTexture = ResourceCache::GetResourceByName<Texture2D>(name);
            SetTexture(unit, dynamic_cast<Texture*>(pTexture));
        }
    }
}