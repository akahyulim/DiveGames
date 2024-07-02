#include "divepch.h"
#include "Material.h"
#include "Graphics/Texture2D.h"
#include "Core/CoreDefs.h"
#include "Resource/ResourceManager.h"

namespace Dive
{
    Material::Material()
        : m_DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_Tiling(1.0f, 1.0f)
        , m_Offset(0.0f, 0.0f)
    {
    }

    Material::~Material()
    {
        DV_ENGINE_TRACE("resource destroy - {0:s}({1:d}), {2:s}({3:d})",
            GetTypeName(), GetTypeHash(), GetName(), GetNameHash());
    }

    bool Material::LoadFromFile(const std::string& fileName)
    {
        return true;
    }

    bool Material::SaveToFile(const std::string& fileName)
    {
        return true;
    }

    Texture* Material::GetTexture(eTextureUnitType unit) const
    {
        auto it = m_Textures.find(unit);

        return it != m_Textures.end() ? it->second : nullptr;
    }
    
    void Material::SetTexture(eTextureUnitType unit, Texture* pTexture)
    {
        if (unit < eTextureUnitType::Count)
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
    void Material::AddTexture(eTextureUnitType unit, const std::string& name)
    {
        if (unit < eTextureUnitType::Count)
        {
            auto pTexture = ResourceManager::GetInstance()->GetResource<Texture2D>(name);
            SetTexture(unit, static_cast<Texture*>(pTexture));
        }
    }
    
    bool Material::HasTexture(eTextureUnitType unit) const
    {
        auto it = m_Textures.find(unit);
        return it != m_Textures.end() && it->second != nullptr;
    }

    Material* LoadMaterialFromFile(const std::string& fileName)
    {
        Material* pObject = new Material;
        if (!pObject->LoadFromFile(fileName))
        {
            DV_DELETE(pObject);
            return nullptr;
        }

        pObject->SetName(fileName);

        return pObject;
    }

    Material* CreateMaterial(const std::string& name)
    {
        if (name.empty())
        {
            DV_ENGINE_ERROR("�߸��� �̸�({:s})�� ���޹޾ҽ��ϴ�.", name);
            return nullptr;
        }

        Material* pObject = new Material();
        pObject->SetName(name);

        return pObject;
    }

    bool Material::IsOpaque() const
    {
        auto it = m_Textures.find(eTextureUnitType::Diffuse);
        if (it != m_Textures.end())
        {
            return it->second->IsOpaque();
        }

        return m_DiffuseColor.w == 1.0f;
    }
}