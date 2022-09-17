#include "divepch.h"
#include "StaticModel.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Graphics/Model.h"
#include "Graphics/Mesh.h"
#include "IO/Log.h"

namespace Dive
{
	StaticModel::StaticModel(Context* pContext)
		: Component(pContext),
		m_pModel(nullptr)
	{
	}

	StaticModel::~StaticModel()
	{
	}

	void StaticModel::SetModel(Model* pModel)
	{
		if (m_pModel == pModel)
			return;

		// 기존 모델 처리 필요?
		m_pModel = pModel;

		if (pModel)
		{
			SetNumMeshes(pModel->GetNumMeshes());
			const auto meshes = pModel->GetMeshes();
			for (unsigned int i = 0; i < (unsigned int)meshes.size(); ++i)
			{
				// 추후 batch등도 설정 필요
				m_Meshes[i] = meshes[i];
			}
		}
		else
		{
			SetNumMeshes(0);
		}
	}

	void StaticModel::SetNumMeshes(unsigned int num)
	{
		m_Meshes.resize(num);
	}

	Mesh* StaticModel::GetMesh(unsigned int index)
	{
		if(index > (unsigned int)m_Meshes.size())
			return nullptr;

		return m_Meshes[index];
	}
}