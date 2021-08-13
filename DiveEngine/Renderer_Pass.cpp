#include "Renderer.h"
#include "GBuffer.h"
#include "Log.h"
#include "GameObject.h"
#include "Camera.h"

namespace dive
{
	// RenderTarget�� ���;� �� �� �ִ�.
	void Renderer::PassGBuffer()
	{
		// ���� ī�޶� ����� �Ŷ��
		// �� ������ ������ ������ �Ѵ�.
	}

	void Renderer::PassLighting()
	{
		// ���� ī�޶� ����� �Ŷ��
		// �� �������� ���� ������ ������ �Ѵ�.

		// ī�޶� RenderTarget�� �ִٸ� �ű⿡�� �׸� ���̴�.
	}

	/*
	* ClearFlags�� ������ ���� �����غ���
	* �ᱹ �ټ��� Camera�� �� ��ǥ�ϴ� �ϳ��� �����ؾ� �� �� ����.
	* ����Ƽ������ �װ� MainCamera�� �� ����.
	* �� �� �˾ƺ���.
	*/
	void Renderer::PassMultiCamTest(ID3D11RenderTargetView* pRTV)
	{
		// ���� ī�޶� ����� �Ŷ��
		// �� ������ ������ ������ �Ѵ�.

		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		// CameraClearFlags Test�� ���� Depth Only�� �����غ��� ������
		// ���� DepthStencilView�� �������� �ʴ´�.

		// �ϴ� Legacy�� �� �� �̻��� Camera�� �׷�����.
		for (auto& pCamera : m_GameObjects[eObjectType::Camera])
		{
			if (m_GameObjects[eObjectType::Opaque].empty())
				return;

			auto pCameraCom = pCamera->GetComponent<Camera>();


			/*
			* �̷����ϸ� �ٷ� �� ȭ���� Clear�Ǿ� ������.
			* �̰� Camera Clear Flags�� ���谡 �ִ°ǰ�?
			float clearColors[4]{ 0.0f, };
			auto color = pCamera->GetComponent<dive::Camera>()->GetBackgroundColor();
			clearColors[0] = color.x;
			clearColors[1] = color.y;
			clearColors[2] = color.z;
			clearColors[3] = color.w;
			immediateContext->ClearRenderTargetView(pRTV, clearColors);
			*/
			immediateContext->IASetInputLayout(m_PipelineStateLegacy.pIL);
			immediateContext->IASetPrimitiveTopology(m_PipelineStateLegacy.primitiveTopology);
			immediateContext->VSSetShader(m_PipelineStateLegacy.pVS, NULL, 0);
			immediateContext->PSSetShader(m_PipelineStateLegacy.pPS, NULL, 0);
			immediateContext->PSSetSamplers(0, 1, &m_PipelineStateLegacy.pSS);
			immediateContext->OMSetDepthStencilState(m_PipelineStateLegacy.pDSS, 1);
			immediateContext->RSSetState(m_PipelineStateLegacy.pRSS);

			// Viewport ���� �ϴ� ������� �����Ѵ�.
			// �׷���... num�� �����ϸ� ��� �Ǵ°���?
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

				meshRenderer->Render(immediateContext);
			}
		}
	}
}