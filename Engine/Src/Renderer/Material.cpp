#include "divepch.h"
#include "Material.h"
#include "Graphics/dvTexture2D.h"

namespace Dive
{
	Material::Material()
	{
		m_Type = eResourceType::Material;
	}

	Material::~Material()
	{
	}

	bool Material::SaveToFile(const std::string& filepath)
	{
		// xml ���Ϸ� �ٷ��.

		// ����� ���?�� ���Ϸ� �����Ѵ�.

		return false;
	}
	
	bool Material::LoadFromFile(const std::string& filepath)
	{
		// xml ���Ϸ� �ٷ��.

		// ������ �ϴ� �����Ѵ�.

		// �ؽ��ĸ� ResourceManager�κ��� �����´�.
		// ���� cache�� ���ٸ� ResourceManager�κ��� Load�Ѵ�.

		return false;
	}
} 