#include "stdafx.h"
#include "Model.h"
#include "Mesh.h"
#include "Core/CoreDefs.h"
//#include "Resource/Importer/ModelLoader.h"

namespace Dive
{
	Model::Model()
	{
	}

	Model::~Model()
	{
		Clear();
	}

	bool Model::LoadFromFile(const std::filesystem::path& path)
	{
		//if (!ModelLoader::Load(this, fileName))
		//	return false;

		return true;
	}
	
	bool Model::SaveToFile(const std::filesystem::path& path)
	{
		return false;
	}

	void Model::Clear()
	{
		for (auto mesh : m_Meshes)
			DV_DELETE(mesh);
		m_Meshes.clear();
	}
	
	Mesh* Model::GetMeshAt(UINT32 index)
	{
		if (index >= static_cast<UINT32>(m_Meshes.size()))
		{
			DV_LOG(Model, warn, "잘못된 메시 컨테이너 인덱스를 전달받았습니다.");
			return nullptr;
		}

		return m_Meshes[index];
	}

	Mesh* Model::GetMeshByName(const std::string& name)
	{
		for (auto mesh : m_Meshes)
		{
			if (mesh->GetName() == name)
				return mesh;
		}

		return nullptr;
	}
}
