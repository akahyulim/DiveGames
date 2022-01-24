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
		// xml 파일로 다룬다.

		// 값들과 경로?를 파일로 저장한다.

		return false;
	}
	
	bool Material::LoadFromFile(const std::string& filepath)
	{
		// xml 파일로 다룬다.

		// 값들을 일단 저장한다.

		// 텍스쳐를 ResourceManager로부터 가져온다.
		// 만약 cache에 없다면 ResourceManager로부터 Load한다.

		return false;
	}
} 