#include "divepch.h"
#include "Drawable.h"
#include "Transform.h"
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
		m_pModel(nullptr)
	{
	}

	Drawable::~Drawable()
	{
		DV_LOG_ENGINE_TRACE("Drawable 소멸 완료({0:d}, {1:s})", m_ID, m_pGameObject->GetName());
	}

	// 실제로는 Update, UpdateBatches, UpdateGeometry 세 가지로 나뉘며
	// 전부 FrameInfo를 전달받는다.
	// 그런데 호출 주체가 쓰레드이다...
	// 즉, Scene에서 GameObject들을 Update하는 구문이 없다.
	void Drawable::Update()
	{
		// UpdateBatches
		// WorldTransform, Distance를 획득한 후 m_SourceDatas에 저장한다.
		// 이는 아래의 SetModel에서도 동일하다.
		{
			auto pTransform = m_pGameObject->GetComponent<Transform>();
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
				// 이외에도 center와
				// DrawalbBatch의 worldTransform을 저장한다.

				// transform을 다시 구현하면서 데이터형이 꼬였다.
				DirectX::XMFLOAT4X4 worldMat;
				DirectX::XMStoreFloat4x4(&worldMat, m_pGameObject->GetComponent<Transform>()->GetMatrix());
				m_SourceDatas[i].WorldTransform = worldMat;

				// 원래는 ResetLodLevels에서 수행한다.
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

	bool Drawable::SetMaterial(size_t index, Material* material)
	{
		if (index >= m_SourceDatas.size())
		{
			DV_LOG_ENGINE_ERROR("Drawable::SetMaterial - 잘못된 인덱스를 전달받았습니다.");
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