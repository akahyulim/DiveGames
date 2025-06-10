#include "stdafx.h"
#include "Material.h"
#include "Graphics/Texture2D.h"
#include "Core/CoreDefs.h"
#include "Resource/ResourceManager.h"

namespace Dive
{
    Material::Material()
    {
		SetName("Material");
    }

    bool Material::LoadFromFile(const std::filesystem::path& filepath)
    {
        // 타입은 enum, 텍스쳐 객체는 이름으로 저장해야 할듯
        return true;
    }

    bool Material::SaveToFile(const std::filesystem::path& filepath)
    {
        return true;
    }

    Texture2D* Material::GetTexture(eTextureUnitType unit) const
    {
        auto it = m_Textures.find(unit);

        return it != m_Textures.end() ? it->second : nullptr;
    }

    // 현재 텍스쳐와 머티리얼 둘 다 Resource 상속 구문을 제거했다.
    // 근데 적어도 텍스쳐 매니져 정도는 필요하다.
    void Material::SetTexture(eTextureUnitType unit, Texture2D* pTexture)
    {
        if (unit < eTextureUnitType::Count)
        {
            if (pTexture)
                m_Textures[unit] = pTexture;
            else
                m_Textures.erase(unit); // 이게 맞나 모르겠다.
        }
    }

    // Renderable의 Update에서 리소스 유무를 파악하는데 사용
    bool Material::HasTexture(eTextureUnitType unit) const
    {
        auto it = m_Textures.find(unit);
        return it != m_Textures.end() && it->second != nullptr;
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
