#include "divepch.h"
#include "Model.h"
#include "Mesh.h"
#include "Core/CoreDefs.h"
#include "Resource/Importer/ModelLoader.h"

namespace Dive
{
	Model::Model()
		: m_pRootObject(nullptr)
	{
	}

	Model::~Model()
	{
		Clear();

		DV_LOG(Model, trace, "resource destroy - {0:s}({1:d}), {2:s}({3:d})",
			GetTypeName(), GetTypeHash(), GetName(), GetNameHash());
	}

	bool Model::LoadFromFile(const std::string& fileName)
	{
		if (!ModelLoader::Load(this, fileName))
			return false;

		return true;
	}
	
	bool Model::SaveToFile(const std::string& fileName)
	{
		return false;
	}

	void Model::Clear()
	{
		for (auto pMesh : m_Meshes)
			DV_DELETE(pMesh);
		m_Meshes.clear();
	}
	
	Mesh* Model::GetMeshAt(uint32_t index)
	{
		if (index >= static_cast<uint32_t>(m_Meshes.size()))
		{
			DV_LOG(Model, warn, "잘못된 메시 컨테이너 인덱스를 전달받았습니다.");
			return nullptr;
		}

		return m_Meshes[index];
	}

	Mesh* Model::GetMeshByName(const std::string& name)
	{
		for (auto pMesh : m_Meshes)
		{
			if (pMesh->GetName() == name)
				return pMesh;
		}

		return nullptr;
	}
}