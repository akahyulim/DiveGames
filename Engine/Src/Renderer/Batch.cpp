#include "divepch.h"
#include "Batch.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Renderer/View.h"
#include "Renderer/Technique.h"
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


	// InstancingBatch에도 사용되므로 public이어야 한다.
	void Batch::Prepare(View* pView)
	{
		if (!m_pVertexShaderVariation || !m_pPixelShaderVariation)
			return;

		pView->GetGraphics()->SetShaders(m_pVertexShaderVariation, m_pPixelShaderVariation);

		// Pass별로
		// graphics에 blend, cull, fill mode등을 전달한다.

		// shaderParameter들을 graphics에 보낸다.
		{
			auto worldTransform = DirectX::XMLoadFloat4x4(&m_WorldTransform);
			worldTransform = DirectX::XMMatrixTranspose(worldTransform);
			pView->GetGraphics()->SetShaderParameter("cModel", worldTransform);
		}

		// material의 텍스쳐를 graphics를 통해 set한다.
		if (m_pMaterial)
		{

		}
	}

	void Batch::Draw(View* pView)
	{
		if (!m_pMesh)
			return;

		Prepare(pView);

		// test parameter
		{
			using namespace DirectX;

			// world
			auto worldMatrix = DirectX::XMMatrixIdentity();

			// view
			XMFLOAT3 up, position, lookAt;
			XMVECTOR upVector, positionVector, lookAtVector;
			float yaw, pitch, roll;
			XMMATRIX rotationMatrix;


			// Setup the vector that points upwards.
			up.x = 0.0f;
			up.y = 1.0f;
			up.z = 0.0f;

			// Load it into a XMVECTOR structure.
			upVector = XMLoadFloat3(&up);

			// Setup the position of the camera in the world.
			position.x = 0.0f;
			position.y = 0.0f;
			position.z = -5.0f;

			// Load it into a XMVECTOR structure.
			positionVector = XMLoadFloat3(&position);

			// Setup where the camera is looking by default.
			lookAt.x = 0.0f;
			lookAt.y = 0.0f;
			lookAt.z = 1.0f;

			// Load it into a XMVECTOR structure.
			lookAtVector = XMLoadFloat3(&lookAt);

			// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
			pitch = 0 * 0.0174532925f;
			yaw = 0 * 0.0174532925f;
			roll = 0 * 0.0174532925f;

			// Create the rotation matrix from the yaw, pitch, and roll values.
			rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

			// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
			lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
			upVector = XMVector3TransformCoord(upVector, rotationMatrix);

			// Translate the rotated camera position to the location of the viewer.
			lookAtVector = XMVectorAdd(positionVector, lookAtVector);

			// Finally create the view matrix from the three updated vectors.
			auto viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

			// proj
			float fieldOfView = 3.141592654f / 4.0f;
			float screenAspect = static_cast<float>(pView->GetGraphics()->GetWidth()) / static_cast<float>(pView->GetGraphics()->GetHeight());
			auto projMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f);

			// constant buffer를 만들어 vs에 전달해야 한다.
			worldMatrix = XMMatrixTranspose(worldMatrix);
			viewMatrix = XMMatrixTranspose(viewMatrix);
			projMatrix = XMMatrixTranspose(projMatrix);

			pView->GetGraphics()->SetShaderParameter("viewMatrix", viewMatrix);
			pView->GetGraphics()->SetShaderParameter("projectionMatrix", projMatrix);
		}

		m_pMesh->Draw(pView->GetGraphics());
	}

	void InstancingBatch::AddTransforms(const Batch& batch)
	{
		//InstanceData data;
		//data.Distance;
		//data.pInstancingData;

		// numWorldTransform만큼 루프를 돌며
		// worldTransform을 저장한 후
		// 완성된 InstanceData를 m_InstanceDatas에 넣는다.
	}
	
	void InstancingBatch::SetInstancingData(void* pLockedData, uint32_t stride, uint32_t& freeIndex)
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

	void InstancingBatch::Draw(View* pView)
	{
		auto pGraphics = pView->GetGraphics();
		auto pRenderer = pView->GetRenderer();

		{
			VertexBuffer* pInstancingBuffer = pRenderer->GetInstancingBuffer();

			// 인스턴스 버퍼가 없거나, 인스턴스 타입이 아니거나, startIndex가 초기값일 경우
			{
				Batch::Prepare(pView);

				// set buffers

				// 기타 + draw
			}
			{
				Batch::Prepare(pView);

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
		m_SortedInstancingBatches.clear();
		m_SortedBatches.clear();
		m_InstancingBatches.clear();
		m_Batches.clear();
	}

	void BatchQueue::Draw(View* pView) const
	{
		auto pGraphics = pView->GetGraphics();
		auto pRenderer = pView->GetRenderer();

		// instanced
		for (auto instancingBatch : m_SortedInstancingBatches)
		{
			// stencil test?

			instancingBatch->Draw(pView);
		}

		// non-instanced
		for (auto batch : m_Batches)//m_SortedBatches)
		{
			// stencil test?

			// scissor test?

			batch.Draw(pView);
		}
	}

	// View의 UpdateGeometry에서 수행하는데 아직 분석을 하지 못했다.
	void BatchQueue::SortBackToFront()
	{
		m_SortedBatches.resize(m_Batches.size());

		for (size_t i = 0; i < m_Batches.size(); ++i)
		{
			m_SortedBatches[i] = &m_Batches[i];
		}

		// renderOlder, distance, sortKey 순으로 정렬한다.
		// 정렬함수를 만들고 Sort 알고리즘에 전달한다.

		// instanced도 정렬한다?
	}

	void BatchQueue::SortFrontToBack()
	{

	}

	// 마지막 인자는 왜 참조일까? 리턴해야 하는 값이면 추후 out을 이름 앞에 붙이자.
	void BatchQueue::SetInstancingData(void* pLockedData, uint32_t stride, uint32_t& freeIndex)
	{
		// 아직 group의 key를 만들지 않았다.
		auto it = m_InstancingBatches.begin();
		for (it; it != m_InstancingBatches.end(); it++)
			it->SetInstancingData(pLockedData, stride, freeIndex);
	}
}