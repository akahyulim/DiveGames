#include "DivePch.h"
#include "Material.h"
#include "Graphics/Texture2D.h"
#include "Resource/ResourceCache.h"
#include "IO/FileStream.h"
#include "IO/ConvertYAML.h"
#include "Core/Log.h"

namespace Dive
{
	Material::Material()
		: Resource(eResourceType::Material),
        m_DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
        m_Tiling(1.0f, 1.0f),
        m_Offset(0.0f, 0.0f)
	{
	}
		
	Material::~Material()
	{
	}

	bool Material::LoadFromFile(const std::string& filePath)
	{
        auto nodes = YAML::LoadAllFromFile(filePath);
        if (nodes.empty())
        {
            DV_CORE_WARN("{:s}�� �������� �ʽ��ϴ�.", filePath);
            return false;
        }

        for (auto node : nodes)
        {
            if (node["Material"])
            {
                auto mat = node["Material"];
                auto albedoMap = mat["DiffuseMap"].as<std::string>();
                auto diffuseColor = mat["DiffuseColor"].as<DirectX::XMFLOAT4>();

                AddTexture(eTextureUnit::Diffuse, albedoMap);
                SetDiffuseColor(diffuseColor);
            }
        }
		
        SetFilePath(filePath);
        SetName(FileSystem::GetFileName(filePath));

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
            Texture2D* pTexture = ResourceCache::GetResourceByPath<Texture2D>(name);    // ���� ByName���� �ߴµ�, ĳ�ÿ��� ã�°Ŷ� ������ �ȵ� �� �־���.
            SetTexture(unit, dynamic_cast<Texture*>(pTexture));
        }
    }
}