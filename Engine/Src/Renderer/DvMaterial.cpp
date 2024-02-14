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
                m_Textures.erase(unit); // �̰� �³� �𸣰ڴ�.
        }
    }

    // urho�� load������ �ؽ��ĸ� �ε��ϱ⿡ ���� �����ô�.
    // �׷��� cache���� GetResource�� �� ���� SetTexture�ϴ°� ���� �ʳ� �ʹ�.
    // ������ �� ������ ����� Cache�� GetResource�� �Ϲ� ���ҽ� ���ϰ� ������ ���� ������
    // �����Ͽ� �ε��� �� �ִٴ� ���̴�.
    void DvMaterial::AddTexture(eTextureUnit unit, const std::string& name)
    {
        if (unit < eTextureUnit::Max_Num)
        {
            // ������ Ÿ�Կ� ���缭 �õ��ؾ� �Ѵ�.
            DvTexture2D* pTexture = ResourceCache::GetResourceByPath<DvTexture2D>(name);    // ���� ByName���� �ߴµ�, ĳ�ÿ��� ã�°Ŷ� ������ �ȵ� �� �־���.
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