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
		const std::string& name = pMesh->GetName();
		if (m_SkinnedMeshes.find(name) != m_SkinnedMeshes.end())
		{
			DV_CORE_ERROR("Model::InsertSkinnedMesh: 이미 존재하는 Mesh({:s})는 추가할 수 없습니다.", name);
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

	// BuildModelPrefab 등의 이름도 괜찮을듯...
	void Model::Build()
	{
		BuildMeshBuffers();

		// 하나의 모델에 동일한 타입의 메시가 둘 이상일 수 있나?
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

		// 추후 이 부분은 Animation으로 넘기는 것이 나아보인다.
		// 대신 Mesh들은 이 곳에서 위와 같이 직접 만드는 형식으로 해야할 듯 하다.
		buildHierarchy(&m_RootNodeInfo, nullptr);
	}

	// 안쓰기로 했다.
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
				DV_CORE_INFO("MeshRenderer를 {:s} GameObject에 추가", nodeName);
				pGameObject->AddComponent<MeshRenderer>()->SetMesh(it->second);
			}
		}

		for (auto it = m_SkinnedMeshes.begin(); it != m_SkinnedMeshes.end(); ++it)
		{
			if (it->second->GetNodeName() == nodeName)
			{
				DV_CORE_INFO("DvSkinnedRenderer를 {:s} GameObject에 추가", nodeName);
				pGameObject->AddComponent<SkinnedMeshRenderer>()->SetMesh(it->second);
			}
		}

		for (uint32_t i = 0; i < pNode->numChildren; ++i)
		{
			buildHierarchy(&pNode->children[i], pTransform);
		}
	}
}