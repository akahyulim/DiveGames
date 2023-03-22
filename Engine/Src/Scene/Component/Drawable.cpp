#include "divepch.h"
#include "Drawable.h"
#include "Transform.h"
#include "Light.h"
#include "Scene/GameObject.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Model.h"
#include "IO/Log.h"

namespace Dive
{
	Drawable::Drawable(Context* pContext)
		: Component(pContext),
		m_pModel(nullptr),
		m_Distance(0.0f)
	{
	}

	Drawable::~Drawable()
	{
		DV_LOG_ENGINE_TRACE("Drawable �Ҹ� �Ϸ�({0:d}, {1:s})", m_ID, m_pGameObject->GetName());
	}

	// �����δ� Update, UpdateBatches, UpdateGeometry �� ������ ������
	// ���� FrameInfo�� ���޹޴´�.
	void Drawable::Update(float delta)
	{
		// UpdateBatches
		// WorldTransform, Distance�� ȹ���� �� m_SourceDatas�� �����Ѵ�.
		// �̴� �Ʒ��� SetModel������ �����ϴ�.
		if (m_pModel)
		{
			auto numMeshes = m_pModel->GetMeshCount();
			//SetNumMesh(numMeshes);

			const auto& meshes = m_pModel->GetMeshes();
			for (uint32_t i = 0; i < numMeshes; ++i)
			{
				//m_Meshes[i] = meshes[i];
				// �̿ܿ��� center��
				// DrawalbBatch�� worldTransform�� �����Ѵ�.

				// transform�� �ٽ� �����ϸ鼭 ���������� ������.
				DirectX::XMFLOAT4X4 worldMat;
				DirectX::XMStoreFloat4x4(&worldMat, m_pGameObject->GetComponent<Transform>()->GetMatrix());
				m_SourceDatas[i].WorldTransform = worldMat;

				// ������ ResetLodLevels���� �����Ѵ�.
				//m_SourceDatas[i].pMesh = meshes[i];
			}
		}
	}

	void Drawable::SetModel(Model* pModel)
	{
		if (m_pModel == pModel)
			return;

		m_pModel = pModel;

		if (pModel)
		{
			auto numMeshes = pModel->GetMeshCount();
			SetNumMesh(numMeshes);

			const auto& meshes = pModel->GetMeshes();
			for (uint32_t i = 0; i < numMeshes; ++i)
			{
				m_Meshes[i] = meshes[i];
				// �̿ܿ��� center��
				// DrawalbBatch�� worldTransform�� �����Ѵ�.

				// transform�� �ٽ� �����ϸ鼭 ���������� ������.
				DirectX::XMFLOAT4X4 worldMat;
				DirectX::XMStoreFloat4x4(&worldMat, m_pGameObject->GetComponent<Transform>()->GetMatrix());
				m_SourceDatas[i].WorldTransform = worldMat;

				// ������ ResetLodLevels���� �����Ѵ�.
				m_SourceDatas[i].pMesh = meshes[i];
			}
		}
		else
		{
			SetNumMesh(0);
		}
	}

	void Drawable::SetMaterial(Material* material)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(m_SourceDatas.size()); ++i)
			m_SourceDatas[i].pMaterial = material;
	}

	Material* Drawable::GetMaterial(size_t index) const
	{
		return index < m_SourceDatas.size() ? m_SourceDatas[index].pMaterial : nullptr;
	}

	void Drawable::AddLight(Light* pLight)
	{
		if (!pLight)
			return;

		// firstLight��°� �ִ�.

		// maxLights�� �ƴϿ��� �߰��Ѵ�.
		m_Lights.emplace_back(pLight);
	}

	bool Drawable::SetMaterial(size_t index, Material* material)
	{
		if (index >= m_SourceDatas.size())
		{
			DV_LOG_ENGINE_ERROR("Drawable::SetMaterial - �߸��� �ε����� ���޹޾ҽ��ϴ�.");
			return false;
		}

		m_SourceDatas[index].pMaterial = material;
		return true;
	}

	void Drawable::SetNumMesh(uint32_t num)
	{
		m_Meshes.resize(num);
		m_SourceDatas.resize(num);
	}

	Mesh* Drawable::GetMesh(size_t batchIndex)
	{
		if (batchIndex < m_SourceDatas.size())
			return m_SourceDatas[batchIndex].pMesh;
		else
			return nullptr;
	}
}