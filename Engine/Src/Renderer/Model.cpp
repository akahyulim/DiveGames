#include "DivePch.h"
#include "Model.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"
#include "Scene/Scene.h"
#include "Scene/Components/MeshRenderer.h"
#include "Scene/Components/SkinnedMeshRenderer.h"

namespace Dive
{
	// ���ĸ�ź�� �ֽ� ���������� ���� �޽��� ����� ���� �޽ö� �̸��� �����ߴ�.
	Model::Model()
		: Resource(eResourceType::Model),
		m_pRootGameObject(nullptr),
		m_pRootBone(nullptr)
	{
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
		// ���ĸ�ź�� ��� 
		// ���� ���� Ȯ������ ��� ���ҽ� ���� ��� ����, �ε��� �о����, ���ؽ� �о���� ������ ������ ��
		// �ٿ�� �ڽ� ���, ũ�� ���� �� ���۸� �����Ѵ�.
		// �ݸ� �ܺ� ���� Ȯ������ ��� �� ������ �� �������� �ε� �Լ��� ȣ���Ѵ�. 
		
		return false;
	}

	// �ӽ�
	// �������� �پ��� ������Ʈ�� ������ ���ӿ�����Ʈ�� ����ȭ�ϴ� ���̴�.
	// �������� �ν���ȭ�Ͽ� ������ ������ ���ӿ�����Ʈ�� ����� �� �ִ�.
	bool Model::SaveToFile(const std::string& filePath)
	{
		// ���ĸ�ź�� ���
		// ���ҽ� ���� ���, �ε�����, ���ؽ��鸸 ���Ͽ� �����Ѵ�.

		// ���ҽ�ĳ�ÿ��� ĳ�� ���� �� ȣ��ȴ�.
		// �̰� �� �����ѵ� ���ҽ�ĳ�ÿ��� �޽� �ε带 ȣ���ϰ� �ٽ� �޽� �ε忡�� �������͸� ȣ���Ѵ�.
		
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
		// ���� ������ �̸����� 2���� skinned mesh�� ������ fbx�� �����Ѵ�....
		std::string name = pMesh->GetName();
		if (m_SkinnedMeshes.find(name) != m_SkinnedMeshes.end())
		{
			DV_CORE_ERROR("Model::InsertSkinnedMesh: �̹� �����ϴ� Mesh({:s})�� �߰��� �� �����ϴ�.", name);
			return nullptr;
		}

		m_SkinnedMeshes[name] = pMesh;

		return m_SkinnedMeshes[name];
	}

	// mesh���� ������ meshrenderer�� �����ͷ� ����Ǿ� �ִ�.
	// ���� �� ������ ���� ���ۿ� ������ ����������.
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