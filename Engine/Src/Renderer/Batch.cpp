#include "divepch.h"
#include "Batch.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Renderer/View.h"
#include "Renderer/Technique.h"
#include "Scene/Component/Camera.h"
#include "IO/Log.h"

namespace Dive
{
	Batch::Batch(const DrawableSourceData& data)
		: m_pMesh(data.pMesh),
		m_pMaterial(data.pMaterial),
		m_WorldTransform(data.WorldTransform),
		m_pPass(nullptr),
		m_pVertexShaderVariation(nullptr),
		m_pPixelShaderVariation(nullptr),
		m_GeometryType(data.GeometryType)
	{}

	void Batch::Prepare(View* pView, Camera* pCamera)
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
			auto viewMatrix = XMMatrixTranspose(pCamera->GetViewMatrix());
			auto projMatrix = XMMatrixTranspose(pCamera->GetProjectionMatrix());

			pView->GetGraphics()->SetShaderParameter("viewMatrix", viewMatrix);
			pView->GetGraphics()->SetShaderParameter("projectionMatrix", projMatrix);
		}
	}

	void Batch::Draw(View* pView, Camera* pCamera)
	{
		if (!m_pMesh)
			return;

		Prepare(pView, pCamera);
		m_pMesh->Draw(pView->GetGraphics());
	}

	void InstancingBatch::AddTransforms(const Batch& batch)
	{
		//InstanceData data;
		//data.Distance;
		//data.pInstancingData;

		// numWorldTransform��ŭ ������ ����
		// worldTransform�� ������ ��
		// �ϼ��� InstanceData�� m_InstanceDatas�� �ִ´�.
	}
	
	void InstancingBatch::SetInstancingData(void* pLockedData, uint32_t stride, uint32_t& freeIndex)
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

	void InstancingBatch::Draw(View* pView, Camera* pCamera)
	{
		auto pGraphics = pView->GetGraphics();
		auto pRenderer = pView->GetRenderer();

		{
			VertexBuffer* pInstancingBuffer = pRenderer->GetInstancingBuffer();

			// �ν��Ͻ� ���۰� ���ų�, �ν��Ͻ� Ÿ���� �ƴϰų�, startIndex�� �ʱⰪ�� ���
			{
				Batch::Prepare(pView, pCamera);

				// set buffers

				// ��Ÿ + draw
			}
			{
				Batch::Prepare(pView, pCamera);

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
		m_SortedInstancingBatches.clear();
		m_SortedBatches.clear();
		m_InstancingBatches.clear();
		m_Batches.clear();
	}

	void BatchQueue::Draw(View* pView, Camera* pCamera) const
	{
		auto pGraphics = pView->GetGraphics();
		auto pRenderer = pView->GetRenderer();

		// instanced
		for (auto instancingBatch : m_SortedInstancingBatches)
		{
			// stencil test?

			instancingBatch->Draw(pView, pCamera);
		}

		// non-instanced
		for (auto batch : m_Batches)//m_SortedBatches)
		{
			// stencil test?

			// scissor test?

			batch.Draw(pView, pCamera);
		}
	}

	// View�� UpdateGeometry���� �����ϴµ� ���� �м��� ���� ���ߴ�.
	void BatchQueue::SortBackToFront()
	{
		m_SortedBatches.resize(m_Batches.size());

		for (size_t i = 0; i < m_Batches.size(); ++i)
		{
			m_SortedBatches[i] = &m_Batches[i];
		}

		// renderOlder, distance, sortKey ������ �����Ѵ�.
		// �����Լ��� ����� Sort �˰��� �����Ѵ�.

		// instanced�� �����Ѵ�?
	}

	void BatchQueue::SortFrontToBack()
	{

	}

	// ������ ���ڴ� �� �����ϱ�? �����ؾ� �ϴ� ���̸� ���� out�� �̸� �տ� ������.
	void BatchQueue::SetInstancingData(void* pLockedData, uint32_t stride, uint32_t& freeIndex)
	{
		// ���� group�� key�� ������ �ʾҴ�.
		auto it = m_InstancingBatches.begin();
		for (it; it != m_InstancingBatches.end(); it++)
			it->SetInstancingData(pLockedData, stride, freeIndex);
	}
}