#include "Renderer.h"
#include "../DiveCore.h"
#include "../Log.h"
#include "../GameObject.h"
#include "../Camera.h"

namespace dive
{
	// ���� �׽�Ʈ ������ �� ���� ���� �־���.
	// ���� ����ȭ��Ų �� ������� �Ѵ�.
	// ��Ƽ ī�޶� �׽�Ʈ�ϴ� �����̴�.
	void Renderer::DrawScene()
	{
		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(immediateContext != nullptr);

		// CameraClearFlags Test�� ���� Depth Only�� �����غ��� ������
		// ���� DepthStencilView�� �������� �ʴ´�.

		// �ϴ� Legacy�� �� �� �̻��� Camera�� �׷�����.
		// => �̰� �� Ư���� ���̽���. ������ Camera�� RenderTexture�� �����ϹǷ� ���� �׷��� �Ѵ�.
		for (auto& pCamera : m_GameObjects[eObjectType::Camera])
		{
			if (m_GameObjects[eObjectType::Opaque].empty())
				return;

			auto pCameraCom = pCamera->GetComponent<Camera>();

			// PipelineState�� ����� ���� �� �̷��� Bind�ϴ°� �³� �ʹ�.
			// �׷��� �ٽú��� ���� �߰� �����Ͱ� �ʿ��ϱ� �ϴ�.
			immediateContext->IASetInputLayout(m_PipelineStateLegacy.pIL);
			immediateContext->IASetPrimitiveTopology(m_PipelineStateLegacy.primitiveTopology);
			immediateContext->VSSetShader(m_PipelineStateLegacy.pVS, NULL, 0);
			immediateContext->PSSetShader(m_PipelineStateLegacy.pPS, NULL, 0);
			immediateContext->PSSetSamplers(0, 1, &m_PipelineStateLegacy.pSS);
			immediateContext->OMSetDepthStencilState(m_PipelineStateLegacy.pDSS, 1);
			immediateContext->RSSetState(m_PipelineStateLegacy.pRSS);

			// Viewport ���� �ϴ� ������� �����Ѵ�.
			// �׷���... num�� �����ϸ� ��� �Ǵ°���?
			//=> �׽�Ʈ������ ��������. Wicked�� Viewport�� RenderPath���� �� �Լ� ȣ������ Bind�Ͽ���. ���� ������ �ٸ��� �ִ�.
			immediateContext->RSSetViewports(1, pCameraCom->GetViewportPtr());

			MeshRenderer* meshRenderer = nullptr;
			for (const auto& gameObject : m_GameObjects[eObjectType::Opaque])
			{
				if (Transform* transform = gameObject->GetTransform())
				{
					// view proj ���� camera�κ��� ���� �����Դ�.
					m_BufferObjectCPU.SetWorldMatrix(transform->GetMatrix());
					m_BufferObjectCPU.SetWorldViewProjectionMatrix(m_BufferObjectCPU.GetWorldMatrix() * pCameraCom->GetViewProjectionMatrix());

					//==========================================================================//
					// Constant Buffer Test														//
					// 1. ���ĸ�ź�� �Լ��� ����� CPU �����͸� GPU�� map / unmap �ߴ�.			//
					// 2. DirectX�� ��İ� HLSL�� ��� ������ �ٸ��⶧���� ��ġ�� �־�� �Ѵ�.	//
					// ���� XMFLOAT4X4�� �����߱⿡ �� �� ����������.							//
					//==========================================================================//
					DirectX::XMMATRIX world = XMMatrixTranspose(m_BufferObjectCPU.GetWorldMatrix());
					DirectX::XMMATRIX wvp = XMMatrixTranspose(m_BufferObjectCPU.GetWorldViewProjectionMatrix());

					// �̰� ������Ʈ ���۴�. UpdateCB���� �������.
					BufferObject* pData = static_cast<BufferObject*>(m_pBufferObjectGPU->Map());
					DirectX::XMStoreFloat4x4(&pData->world, world);
					DirectX::XMStoreFloat4x4(&pData->wvp, wvp);
					assert(m_pBufferObjectGPU->Unmap());

					ID3D11Buffer* buffer = m_pBufferObjectGPU->GetBuffer();
					immediateContext->VSSetConstantBuffers(0, 1, &buffer);
				}

				meshRenderer = gameObject->GetComponent<MeshRenderer>();
				if (!meshRenderer)
					continue;

				// �̰� �ٽ� Draw�� �ٲ�� �� �� ����.
				meshRenderer->Render(immediateContext);
			}
		}
	}

	void Renderer::DrawLight()
	{

	}
}