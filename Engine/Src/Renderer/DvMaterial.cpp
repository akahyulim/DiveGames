#include "divepch.h"
#include "DvMaterial.h"
#include "Graphics/DvShader.h"
#include "Graphics/DvTexture2D.h"
#include "Core/CoreDefs.h"
#include "Resource/ResourceCache.h"

namespace Dive
{
	DvMaterial::DvMaterial()
		: Resource(eResourceType::DvMaterial)
        , m_pShader(nullptr)
        , m_DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_Tiling(1.0f, 1.0f)
        , m_Offset(0.0f, 0.0f)
	{
	}

	DvMaterial::DvMaterial(const std::string& name, DvShader* pShader)
		: Resource(eResourceType::DvMaterial)
        , m_pShader(pShader)
        , m_DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_Tiling(1.0f, 1.0f)
        , m_Offset(0.0f, 0.0f)
	{
        SetName(name);
	}

	DvMaterial::~DvMaterial()
	{
	}

    DvTexture* DvMaterial::GetTexture(eTextureUnit unit) const
    {
        auto it = m_Textures.find(unit);

        return it != m_Textures.end() ? it->second : nullptr;
    }

    void DvMaterial::SetTexture(eTextureUnit unit, DvTexture* pTexture)
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
    void DvMaterial::AddTexture(eTextureUnit unit, const std::string& name)
    {
        if (unit < eTextureUnit::Max_Num)
        {
            // 원래는 타입에 맞춰서 시도해야 한다.
            DvTexture2D* pTexture = ResourceCache::GetResourceByPath<DvTexture2D>(name);    // 원래 ByName으로 했는데, 캐시에서 찾는거라 적용이 안될 수 있었다.
            SetTexture(unit, dynamic_cast<DvTexture*>(pTexture));
        }
    }

    bool DvMaterial::HasTexture(eTextureUnit unit)
    {
        return m_Textures[unit] != nullptr;
    }

    DvMaterial* DvMaterial::Create(const std::string& name, DvShader* pShader)
    {
        return new DvMaterial(name, pShader);
    }
}