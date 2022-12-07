#include "divepch.h"
#include "Drawable.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Renderer/Model.h"
#include "IO/Log.h"

namespace Dive
{
	Drawable::Drawable(Context* pContext)
		: Component(pContext)
	{
	}

	Drawable::~Drawable()
	{
	}

	void Drawable::SetModel(Model* pModel)
	{
		if (m_pModel == pModel)
			return;

		m_pModel = pModel;

		if (pModel)
		{
			auto numMeshes = pModel->GetNumMeshes();
			SetNumMesh(numMeshes);

			const auto& meshes = pModel->GetMeshes();
			for (unsigned int i = 0; i < numMeshes; ++i)
			{
				m_Meshes[i] = meshes[i];
				// 이외에도 center와
				// DrawalbBatch의 worldTransform을 저장한다.

				// 원래는 ResetLodLevels에서 수행한다.
				m_SourceDatas[i].m_pMesh = meshes[i];
			}
		}
		else
		{
			SetNumMesh(0);
		}
	}

	void Drawable::SetMaterial(Material* pMaterial)
	{
		for (unsigned int i = 0; i < static_cast<unsigned int>(m_SourceDatas.size()); ++i)
			m_SourceDatas[i].m_pMaterial = pMaterial;
	}

	Material* Drawable::GetMaterial(size_t index) const
	{
		return index < m_SourceDatas.size() ? m_SourceDatas[index].m_pMaterial : nullptr;
	}

	bool Drawable::SetMaterial(size_t index, Material* pMaterial)
	{
		if (index >= m_SourceDatas.size())
		{
			DV_LOG_ENGINE_ERROR("Drawable::SetMaterial - 잘못된 인덱스를 전달받았습니다.");
			return false;
		}

		m_SourceDatas[index].m_pMaterial = pMaterial;
		return true;
	}

	void Drawable::SetNumMesh(unsigned int num)
	{
		m_Meshes.resize(num);
		m_SourceDatas.resize(num);
	}

	Mesh* Drawable::GetMesh(size_t batchIndex)
	{
		if (batchIndex < m_SourceDatas.size())
			return m_SourceDatas[batchIndex].m_pMesh;
		else
			return nullptr;
	}
}