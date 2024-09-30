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

    Texture2D* Material::GetDvTexture(eTextureUnitType unit) const
    {
        auto it = m_DvTextures.find(unit);

        return it != m_DvTextures.end() ? it->second : nullptr;
    }

    void Material::SetDvTexture(eTextureUnitType unit, Texture2D* pTexture)
    {
        if (unit < eTextureUnitType::Count)
        {
            if (pTexture)
                m_DvTextures[unit] = pTexture;
            else
                m_DvTextures.erase(unit); // 이게 맞나 모르겠다.
        }
    }

    // Renderable의 Update에서 리소스 유무를 파악하는데 사용
    bool Material::HasTexture(eTextureUnitType unit) const
    {
        auto it = m_DvTextures.find(unit);
        return it != m_DvTextures.end() && it->second != nullptr;
    }

    // urho는 load에서만 텍스쳐를 로드하기에 직접 만들어봤다.
   // 그런데 cache에서 GetResource한 후 위의 SetTexture하는게 맞지 않나 싶다.
   // 하지만 이 구현이 힘든게 Cache의 GetResource는 일반 리소스 파일과 엔진의 설정 파일을
   // 구분하여 로드할 수 있다는 것이다.
    void Material::AddTexture(eTextureUnitType unit, const std::string& name)
    {
        if (unit < eTextureUnitType::Count)
        {
            auto pTexture = ResourceManager::GetInstance()->GetResource<Texture2D>(name);
            SetDvTexture(unit, pTexture);
        }
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
            DV_ENGINE_ERROR("잘못된 이름({:s})을 전달받았습니다.", name);
            return nullptr;
        }

        Material* pObject = new Material();
        pObject->SetName(name);

        return pObject;
    }

    bool Material::IsOpaque() const
    {
        auto it = m_DvTextures.find(eTextureUnitType::Diffuse);
        if (it != m_DvTextures.end())
        {
            return it->second->IsOpaque();
        }

        return m_DiffuseColor.w == 1.0f;
    }
}