#include "DivePch.h"
#include "Model.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

#include "Scene/Scene.h"
//#include "Scene/GameObject.h"
//#include "Scene/Components/Transform.h"
#include "Scene/Components/MeshRenderer.h"
#include "Scene/Components/SkinnedMeshRenderer.h"

namespace Dive
{
	Model::Model()
		: Resource(eResourceType::Model),
		m_pRootGameObject(nullptr)
	{
		//m_pRootGameObject = Scene::CreateGameObject();
	}

	Model::~Model()
	{
		// GameObject�� ������ Update���� ����������
		Scene::RemoveGameObject(m_pRootGameObject);

		// Mesh�� Buffer�� ��� �����ȴ�.
		// ������ View���� �� ����� �� �� �����Ƿ� ���� ������ �߻��ϴ� ���̴�.
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
			DV_CORE_ERROR("Model::InsertStaticMesh: �̹� �����ϴ� Mesh({:s})�� �߰��� �� �����ϴ�.",name);
			return nullptr;
		}

		m_StaticMeshes[name] = pMesh;

		return m_StaticMeshes[name];
	}

	SkinnedMesh* Model::InsertSkinnedMesh(SkinnedMesh* pMesh)
	{
		const std::string& name = pMesh->GetName();
		if (m_SkinnedMeshes.find(name) != m_SkinnedMeshes.end())
		{
			DV_CORE_ERROR("Model::InsertSkinnedMesh: �̹� �����ϴ� Mesh({:s})�� �߰��� �� �����ϴ�.", name);
			return nullptr;
		}

		m_SkinnedMeshes[name] = pMesh;

		return m_SkinnedMeshes[name];
	}

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

	// BuildModelPrefab ���� �̸��� ��������...
	void Model::Build()
	{
		BuildMeshBuffers();

		// �ϳ��� �𵨿� ������ Ÿ���� �޽ð� �� �̻��� �� �ֳ�?
		/*
		m_pRootGameObject = Scene::CreateGameObject();
		for (auto& mesh : m_StaticMeshes)
		{
			auto pMeshRenderer = m_pRootGameObject->AddComponent<MeshRenderer>();
			pMeshRenderer->SetMesh(&mesh.second);
		}
		for (auto& mesh : m_SkinnedMeshes)
		{
			auto pMeshRenderer = m_pRootGameObject->AddComponent<SkinnedMeshRenderer>();
			pMeshRenderer->SetMesh(&mesh.second);
		}
		*/

		// ���� �� �κ��� Animation���� �ѱ�� ���� ���ƺ��δ�.
		// ��� Mesh���� �� ������ ���� ���� ���� ����� �������� �ؾ��� �� �ϴ�.
		buildHierarchy(&m_RootNodeInfo, nullptr);
	}

	// �Ⱦ���� �ߴ�.
	void Model::buildHierarchy(NodeInfo* pNode, Transform* pParent)
	{
		std::string nodeName = pNode->name;
		GameObject* pGameObject = Scene::CreateGameObject(nodeName);
		
		if (!pParent)
		{
			m_pRootGameObject = pGameObject;
		}

		Transform* pTransform = pGameObject->GetTransform();
		pTransform->SetLocalMatrix(pNode->transform);
		pTransform->SetParent(pParent);

		for (auto it = m_StaticMeshes.begin(); it != m_StaticMeshes.end(); ++it)
		{
			if (it->second->GetNodeName() == nodeName)
			{
				DV_CORE_INFO("MeshRenderer�� {:s} GameObject�� �߰�", nodeName);
				pGameObject->AddComponent<MeshRenderer>()->SetMesh(it->second);
			}
		}

		for (auto it = m_SkinnedMeshes.begin(); it != m_SkinnedMeshes.end(); ++it)
		{
			if (it->second->GetNodeName() == nodeName)
			{
				DV_CORE_INFO("DvSkinnedRenderer�� {:s} GameObject�� �߰�", nodeName);
				pGameObject->AddComponent<SkinnedMeshRenderer>()->SetMesh(it->second);
			}
		}

		for (uint32_t i = 0; i < pNode->numChildren; ++i)
		{
			buildHierarchy(&pNode->children[i], pTransform);
		}
	}
}