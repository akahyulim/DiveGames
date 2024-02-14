#include "divepch.h"
#include "Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Core/CoreDefs.h"
#include "Resource/ResourceCache.h"

namespace Dive
{
	Material::Material()
		: Resource(eResourceType::Material)
        , m_pShader(nullptr)
        , m_DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_Tiling(1.0f, 1.0f)
        , m_Offset(0.0f, 0.0f)
	{
	}

	Material::Material(const std::string& name, Shader* pShader)
		: Resource(eResourceType::Material)
        , m_pShader(pShader)
        , m_DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
        , m_Tiling(1.0f, 1.0f)
        , m_Offset(0.0f, 0.0f)
	{
        SetName(name);
	}

	Material::~Material()
	{
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
            Texture2D* pTexture = ResourceCache::GetResourceByPath<Texture2D>(name);    // ���� ByName���� �ߴµ�, ĳ�ÿ��� ã�°Ŷ� ������ �ȵ� �� �־���.
            SetTexture(unit, dynamic_cast<Texture*>(pTexture));
        }
    }

    bool Material::HasTexture(eTextureUnit unit)
    {
        return m_Textures[unit] != nullptr;
    }

    Material* Material::Create(const std::string& name, Shader* pShader)
    {
        return new Material(name, pShader);
    }
}