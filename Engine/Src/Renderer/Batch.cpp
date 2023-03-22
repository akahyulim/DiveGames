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

		// Pass별로
		// graphics에 blend, cull, fill mode등을 전달한다.

		// shaderParameter들을 graphics에 보낸다.
		{
			auto worldTransform = DirectX::XMLoadFloat4x4(&m_WorldTransform);
			worldTransform = DirectX::XMMatrixTranspose(worldTransform);
			pGraphics->SetShaderParameter("cModel", worldTransform);
		}

		// 이건 다양한 모드들
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

		// 이건 셰이더 파라미터 & 텍스쳐
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
			auto cameraPos = pCamera->GetGameObject()->GetComponent<Transform>()->GetPosition();	// 상당히 번거롭다...
			auto viewMatrix = XMMatrixTranspose(pCamera->GetViewMatrix());
			auto projMatrix = XMMatrixTranspose(pCamera->GetProjectionMatrix());

			// 그냥 전달만 하면 각각의 셰이더에서 사용하는 변수에 넣어준다.
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

		// numWorldTransform만큼 루프를 돌며
		// worldTransform을 저장한 후
		// 완성된 InstanceData를 m_InstanceDatas에 넣는다.
	}
	
	void InstanceBatch::SetInstanceData(void* pLockedData, uint32_t stride, uint32_t& freeIndex)
	{
		if (m_GeometryType != eGeometryType::Instanced)
			return;

		// 이게 좀 중요하다. 일단 외부에서 받아온다.
		m_StartIndex = freeIndex;
		// 락된 포인터에서 정확한 위치를 설정한다.

		// instances만큼 루프를 돌려
		// transform과 instancingData를 추가한다

		// 최종적으로 freeIndex를 갱신한다.
	}

	void InstanceBatch::Draw(View* pView, Camera* pCamera)
	{
		auto pGraphics = pView->GetGraphics();
		auto pRenderer = pView->GetRenderer();

		{
			VertexBuffer* pInstancingBuffer = pRenderer->GetInstancingBuffer();

			// 인스턴스 버퍼가 없거나, 인스턴스 타입이 아니거나, startIndex가 초기값일 경우
			{
				StaticBatch::Prepare(pView, pCamera);

				// set buffers

				// 기타 + draw
			}
			{
				StaticBatch::Prepare(pView, pCamera);

				// Graphics에서 VertexBuffer vector 획득
				// 이건 아마도 이미 VertexBuffer를 bind한 상태일 것이다.
				// 따라서 이 다음 배열에 InstancingBuffer를 추가한다.

				// buffers bind

				// DrawInstanced

				// instancingBuffer를 VertexBuffer에서 pop하는 듯 하다.
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

	// View의 UpdateGeometry에서 수행하는데 아직 분석을 하지 못했다.
	void BatchQueue::SortBackToFront()
	{
		m_SortedStaticBatches.resize(m_StaticBatches.size());

		for (size_t i = 0; i < m_StaticBatches.size(); ++i)
		{
			m_SortedStaticBatches[i] = &m_StaticBatches[i];
		}

		// renderOlder, distance, sortKey 순으로 정렬한다.
		// 정렬함수를 만들고 Sort 알고리즘에 전달한다.

		// instanced도 정렬한다?
	}

	void BatchQueue::SortFrontToBack()
	{

	}

	// 마지막 인자는 왜 참조일까? 리턴해야 하는 값이면 추후 out을 이름 앞에 붙이자.
	void BatchQueue::SetInstanceData(void* pLockedData, uint32_t stride, uint32_t& freeIndex)
	{
		// 아직 group의 key를 만들지 않았다.
		auto it = m_InstanceBatches.begin();
		for (it; it != m_InstanceBatches.end(); it++)
			it->SetInstanceData(pLockedData, stride, freeIndex);
	}
}