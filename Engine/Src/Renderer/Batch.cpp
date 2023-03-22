#include "divepch.h"
#include "Batch.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Renderer/View.h"
#include "Renderer/Technique.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Light.h"
#include "IO/Log.h"

namespace Dive
{
	StaticBatch::StaticBatch(const DrawableSourceData& data)
		: m_pMesh(data.pMesh),
		m_pMaterial(data.pMaterial),
		m_WorldTransform(data.WorldTransform),
		m_pPass(nullptr),
		m_pVertexShaderVariation(nullptr),
		m_pPixelShaderVariation(nullptr),
		m_GeometryType(data.GeometryType),
		m_pLightBatchQueue(nullptr)
	{}

	void StaticBatch::Prepare(View* pView, Camera* pCamera)
	{
		if (!m_pVertexShaderVariation || !m_pPixelShaderVariation)
			return;

		auto* pGraphics = pView->GetGraphics();
		pGraphics->SetShaders(m_pVertexShaderVariation, m_pPixelShaderVariation);

		// Pass����
		// graphics�� blend, cull, fill mode���� �����Ѵ�.

		// shaderParameter���� graphics�� ������.
		{
			auto worldTransform = DirectX::XMLoadFloat4x4(&m_WorldTransform);
			worldTransform = DirectX::XMMatrixTranspose(worldTransform);
			pGraphics->SetShaderParameter("cModel", worldTransform);
		}

		// �̰� �پ��� ����
		if (m_pPass && m_pMaterial)
		{
			// blend mode

			// cull mode

			// fill mode
		}

		// light
		if (m_pLightBatchQueue)
		{
			Light* pLight = m_pLightBatchQueue->pLight;

			pView->GetGraphics()->SetShaderParameter("lightColor", pLight->GetColor());
			pView->GetGraphics()->SetShaderParameter("lightDir", pLight->GetDir());
		}
		else
		{
			pView->GetGraphics()->SetShaderParameter("lightColor", DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));// 0.5f, 0.75f, 1.0f));
			pView->GetGraphics()->SetShaderParameter("lightDir", DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f));
		}

		// �̰� ���̴� �Ķ���� & �ؽ���
		if (m_pMaterial)
		{
			const auto& textures = m_pMaterial->GetTextures();
			for (auto it = textures.begin(); it != textures.end(); ++it)
			{
				if (pGraphics->HasTextureUnit(it->first))
					pGraphics->SetTexture(static_cast<uint32_t>(it->first), it->second);

			}
		}

		// camera
		{
			auto cameraPos = pCamera->GetGameObject()->GetComponent<Transform>()->GetPosition();	// ����� ���ŷӴ�...
			auto viewMatrix = XMMatrixTranspose(pCamera->GetViewMatrix());
			auto projMatrix = XMMatrixTranspose(pCamera->GetProjectionMatrix());

			// �׳� ���޸� �ϸ� ������ ���̴����� ����ϴ� ������ �־��ش�.
			pView->GetGraphics()->SetShaderParameter("cameraPos", cameraPos);
			pView->GetGraphics()->SetShaderParameter("viewMatrix", viewMatrix);
			pView->GetGraphics()->SetShaderParameter("projectionMatrix", projMatrix);
		}
	}

	void StaticBatch::Draw(View* pView, Camera* pCamera)
	{
		if (!m_pMesh)
			return;

		Prepare(pView, pCamera);
		m_pMesh->Draw(pView->GetGraphics());
	}

	void InstanceBatch::AddTransforms(const StaticBatch& batch)
	{
		//InstanceData data;
		//data.Distance;
		//data.pInstanceData;

		// numWorldTransform��ŭ ������ ����
		// worldTransform�� ������ ��
		// �ϼ��� InstanceData�� m_InstanceDatas�� �ִ´�.
	}
	
	void InstanceBatch::SetInstanceData(void* pLockedData, uint32_t stride, uint32_t& freeIndex)
	{
		if (m_GeometryType != eGeometryType::Instanced)
			return;

		// �̰� �� �߿��ϴ�. �ϴ� �ܺο��� �޾ƿ´�.
		m_StartIndex = freeIndex;
		// ���� �����Ϳ��� ��Ȯ�� ��ġ�� �����Ѵ�.

		// instances��ŭ ������ ����
		// transform�� instancingData�� �߰��Ѵ�

		// ���������� freeIndex�� �����Ѵ�.
	}

	void InstanceBatch::Draw(View* pView, Camera* pCamera)
	{
		auto pGraphics = pView->GetGraphics();
		auto pRenderer = pView->GetRenderer();

		{
			VertexBuffer* pInstancingBuffer = pRenderer->GetInstancingBuffer();

			// �ν��Ͻ� ���۰� ���ų�, �ν��Ͻ� Ÿ���� �ƴϰų�, startIndex�� �ʱⰪ�� ���
			{
				StaticBatch::Prepare(pView, pCamera);

				// set buffers

				// ��Ÿ + draw
			}
			{
				StaticBatch::Prepare(pView, pCamera);

				// Graphics���� VertexBuffer vector ȹ��
				// �̰� �Ƹ��� �̹� VertexBuffer�� bind�� ������ ���̴�.
				// ���� �� ���� �迭�� InstancingBuffer�� �߰��Ѵ�.

				// buffers bind

				// DrawInstanced

				// instancingBuffer�� VertexBuffer���� pop�ϴ� �� �ϴ�.
			}
		}
	}

	void BatchQueue::Clear()
	{
		m_SortedInstanceBatches.clear();
		m_SortedStaticBatches.clear();
		m_InstanceBatches.clear();
		m_StaticBatches.clear();
	}

	void BatchQueue::Draw(View* pView, Camera* pCamera) const
	{
		auto pGraphics = pView->GetGraphics();
		auto pRenderer = pView->GetRenderer();

		// instanced
		for (auto instancingBatch : m_SortedInstanceBatches)
		{
			// stencil test?

			instancingBatch->Draw(pView, pCamera);
		}

		// non-instanced
		for (auto staticBatch : m_StaticBatches)//m_SortedStaticBatches)
		{
			// stencil test?

			// scissor test?

			staticBatch.Draw(pView, pCamera);
		}
	}

	// View�� UpdateGeometry���� �����ϴµ� ���� �м��� ���� ���ߴ�.
	void BatchQueue::SortBackToFront()
	{
		m_SortedStaticBatches.resize(m_StaticBatches.size());

		for (size_t i = 0; i < m_StaticBatches.size(); ++i)
		{
			m_SortedStaticBatches[i] = &m_StaticBatches[i];
		}

		// renderOlder, distance, sortKey ������ �����Ѵ�.
		// �����Լ��� ����� Sort �˰��� �����Ѵ�.

		// instanced�� �����Ѵ�?
	}

	void BatchQueue::SortFrontToBack()
	{

	}

	// ������ ���ڴ� �� �����ϱ�? �����ؾ� �ϴ� ���̸� ���� out�� �̸� �տ� ������.
	void BatchQueue::SetInstanceData(void* pLockedData, uint32_t stride, uint32_t& freeIndex)
	{
		// ���� group�� key�� ������ �ʾҴ�.
		auto it = m_InstanceBatches.begin();
		for (it; it != m_InstanceBatches.end(); it++)
			it->SetInstanceData(pLockedData, stride, freeIndex);
	}
}