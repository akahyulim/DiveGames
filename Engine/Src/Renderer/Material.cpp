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
                m_Textures.erase(unit); // �̰� �³� �𸣰ڴ�.
        }
    }

    // urho�� load������ �ؽ��ĸ� �ε��ϱ⿡ ���� �����ô�.
    // �׷��� cache���� GetResource�� �� ���� SetTexture�ϴ°� ���� �ʳ� �ʹ�.
    // ������ �� ������ ����� Cache�� GetResource�� �Ϲ� ���ҽ� ���ϰ� ������ ���� ������
    // �����Ͽ� �ε��� �� �ִٴ� ���̴�.
    void Material::AddTexture(eTextureUnit unit, const std::string& name)
    {
        if (unit < eTextureUnit::Max_Num)
        {
            // ������ Ÿ�Կ� ���缭 �õ��ؾ� �Ѵ�.
            Texture2D* pTexture = ResourceCache::GetResourceByName<Texture2D>(name);
            SetTexture(unit, dynamic_cast<Texture*>(pTexture));
        }
    }
}