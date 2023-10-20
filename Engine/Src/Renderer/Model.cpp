#include "DivePch.h"
#include "Model.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"
#include "Scene/Scene.h"
#include "Scene/Components/MeshRenderer.h"
#include "Scene/Components/SkinnedMeshRenderer.h"

namespace Dive
{
	Model::Model()
		: Resource(eResourceType::Model),
		m_pRootGameObject(nullptr),
		m_pRootBone(nullptr)
	{
	}

	Model::~Model()
	{
		// GameObject는 다음번 Update에서 정리되지만
		Scene::RemoveGameObject(m_pRootGameObject);

		// Mesh와 Buffer는 즉시 삭제된다.
		// 하지만 View에선 이 사실을 알 수 없으므로 접근 오류가 발생하는 것이다.
		{
			auto it = m_StaticMeshes.begin();
			for (it; it != m_StaticMeshes.end(); ++it)
			{
				DV_DELETE(it->second);
			}
			m_StaticMeshes.clear();
		}

		{
			auto it = m_SkinnedMeshes.begin();
			for (it; it != m_SkinnedMeshes.end(); ++it)
			{
				DV_DELETE(it->second);
			}
			m_SkinnedMeshes.clear();
		}
	}

	bool Model::LoadFromFile(const std::string& filePath)
	{
		return false;
	}

	bool Model::SaveToFile(const std::string& filePath)
	{
		return false;
	}
	
	StaticMesh* Model::InsertStaticMesh(StaticMesh* pMesh)
	{
		const std::string& name = pMesh->GetName();
		if (m_StaticMeshes.find(name) != m_StaticMeshes.end())
		{
			DV_CORE_ERROR("Model::InsertStaticMesh: 이미 존재하는 Mesh({:s})는 추가할 수 없습니다.",name);
			return nullptr;
		}

		m_StaticMeshes[name] = pMesh;

		return m_StaticMeshes[name];
	}

	SkinnedMesh* Model::InsertSkinnedMesh(SkinnedMesh* pMesh)
	{
		// 현재 동일한 이름으로 2개의 skinned mesh를 가지는 fbx가 존재한다....
		std::string name = pMesh->GetName();
		if (m_SkinnedMeshes.find(name) != m_SkinnedMeshes.end())
		{
			DV_CORE_ERROR("Model::InsertSkinnedMesh: 이미 존재하는 Mesh({:s})는 추가할 수 없습니다.", name);
			return nullptr;
		}

		m_SkinnedMeshes[name] = pMesh;

		return m_SkinnedMeshes[name];
	}

	// mesh들은 각각의 meshrenderer에 포인터로 저장되어 있다.
	// 따라서 이 곳에서 만들어도 버퍼에 접근이 가능해진다.
	void Model::BuildMeshBuffers()
	{
		if (m_StaticMeshes.empty() && m_SkinnedMeshes.empty())
			return;

		for (auto& mesh : m_StaticMeshes)
		{
			mesh.second->CreateVertexBuffer();
			mesh.second->CreateIndexBuffer();
		}

		for (auto& mesh : m_SkinnedMeshes)
		{
			mesh.second->CreateVertexBuffer();
			mesh.second->CreateIndexBuffer();
		}
	}

	void Model::Build()
	{
		BuildMeshBuffers();
	}
}