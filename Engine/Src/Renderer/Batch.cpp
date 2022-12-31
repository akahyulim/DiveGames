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


	// InstancingBatch���� ���ǹǷ� public�̾�� �Ѵ�.
	void Batch::Prepare(View* pView)
	{
		if (!m_pVertexShaderVariation || !m_pPixelShaderVariation)
			return;

		pView->GetGraphics()->SetShaders(m_pVertexShaderVariation, m_pPixelShaderVariation);

		// Pass����
		// graphics�� blend, cull, fill mode���� �����Ѵ�.

		// shaderParameter���� graphics�� ������.
		{
			auto worldTransform = DirectX::XMLoadFloat4x4(&m_WorldTransform);
			worldTransform = DirectX::XMMatrixTranspose(worldTransform);
			pView->GetGraphics()->SetShaderParameter("cModel", worldTransform);
		}

		// material�� �ؽ��ĸ� graphics�� ���� set�Ѵ�.
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

			// constant buffer�� ����� vs�� �����ؾ� �Ѵ�.
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

	void InstancingBatch::Draw(View* pView)
	{
		auto pGraphics = pView->GetGraphics();
		auto pRenderer = pView->GetRenderer();

		{
			VertexBuffer* pInstancingBuffer = pRenderer->GetInstancingBuffer();

			// �ν��Ͻ� ���۰� ���ų�, �ν��Ͻ� Ÿ���� �ƴϰų�, startIndex�� �ʱⰪ�� ���
			{
				Batch::Prepare(pView);

				// set buffers

				// ��Ÿ + draw
			}
			{
				Batch::Prepare(pView);

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