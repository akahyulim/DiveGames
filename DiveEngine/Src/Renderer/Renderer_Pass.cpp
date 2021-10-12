#include "Renderer.h"
#include "../DiveCore.h"
#include "../Log.h"
#include "../GameObject.h"
#include "../Camera.h"

// �ϴ� dvRenderer ������ �������� �̷��
// �� ������ Deferred Shader�� Lighting ���� �����غ���.
namespace dive
{
	// Multi-Thread Render reference�� �������ڸ�
	// �� ������ Static�� Dynamic Params�� �����ϰ�
	// RenderScene() ���� ������ Bind�� ��ų �� �ִ�.
	// �׷����� Pass�� Params�� ������ ���� ��ü Ȥ�� ������ �ʿ��ϴ�.
	// static�� ��� RenderPath���� RTV, DSV ���� ������ �� ���� �� �����ѵ�
	// Dynamic�� ��쿣 ���� �ؾ� �ұ�?
	void Renderer::DrawScene()
	{
		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(immediateContext != nullptr);

		// CameraClearFlags Test�� ���� Depth Only�� �����غ��� ������
		// ���� DepthStencilView�� �������� �ʴ´�.

		// ����Ƽ�� ���� ī�޶� ������ �����غ� �����̴�.
		// �̸� �״�� �����Ϸ��� ��� MainCamera ������ ������ �ʿ��ϴ�.
		// �׸��� Viewport ������ �ʿ��� �۾��� ���ο��� �� �־�� �Ѵ�.
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
			//immediateContext->PSSetSamplers(0, 1, &m_PipelineStateLegacy.pSS);
			immediateContext->PSSetSamplers(0, 1, m_pSamplerStates[(size_t)eSamplerState::Linear].GetAddressOf());
			immediateContext->OMSetDepthStencilState(m_PipelineStateLegacy.pDSS, 1);
			immediateContext->RSSetState(m_PipelineStateLegacy.pRSS);

			// Viewport ���� �ϴ� ������� �����Ѵ�.
			// �׷���... num�� �����ϸ� ��� �Ǵ°���?
			// ���� num�� MainCamera ������ŭ, Viewport �����͸� �迭�� ������ �� �ִٸ�
			// ī�޶� ������ŭ ������ ������ �ʾƵ� �� ���̴�.
			// �ٸ� �� ��� ��ȯ ����� ���� ������ �� �־�� �Ѵ�.
			//immediateContext->RSSetViewports(1, pCameraCom->GetViewportPtr());

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
				// �׷��� AssetData Bind�� �߰��� �� �������� ���̴�.
				meshRenderer->Render(immediateContext);
			}
		}
	}

	// ���߿� DrawScene�� ����� �̸��� �ٲ���.
	void Renderer::DrawGBuffer()
	{
		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(immediateContext != nullptr);

		// m_GameObjects�� ���ŵ��� �ʴ� ��찡 �߻��߾���.
		// ������ ������ �𸣰ڴ�.
		for (auto& pCamera : m_GameObjects[eObjectType::Camera])
		{
			if (m_GameObjects[eObjectType::Opaque].empty())
				return;

			auto pCameraCom = pCamera->GetComponent<Camera>();

	
			// ��� GET �Լ��� �ִ�...
			// �׷��� ���� ������ Ÿ�Զ����� ���� �������°� ���ϴ�.
			immediateContext->VSSetShader((ID3D11VertexShader*)m_pShaders[SHADERTYPES::VSTYPE_DEFERRED_SHADING].Get(), NULL, 0);
			immediateContext->PSSetShader((ID3D11PixelShader*)m_pShaders[SHADERTYPES::PSTYPE_DEFERRED_SHADING].Get(), NULL, 0);
			immediateContext->IASetInputLayout(m_pInputLayouts[ILTYPES::ILTYPE_POS_TEX_NOR_TAN].Get());
			// ���� ����, ����, �����ͷ� ������ ���� �� �ִ�.
			immediateContext->PSSetSamplers(0, 1, m_pSamplerStates[(size_t)eSamplerState::Linear].GetAddressOf());
			immediateContext->OMSetDepthStencilState(m_pDepthStencilStates[(size_t)eDepthStencilState::Default].Get(), 1);
			immediateContext->RSSetState(m_pRasterizerStates[(size_t)eRasterizerState::Cullback_Solid].Get());
			immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

					// ���� �ٷ� �� DrawScene()�� ������ �ڵ�������
					// �� cb�� DeferredShading���� ���Ǿ�� �ϴ� cb�̴�.
					// �� ������ cb�� ������ wvp, world�� �Ǿ� �����Ƿ�, ������ ������ �߻��� �� �ִ�.
					// ���� shader�� ������ �ٲٴ� ���� �ξ� �����ϰ�����, �� ���� �� offset ������ ������ �߻��� �� �ִ�.
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
				// �׷��� AssetData Bind�� �߰��� �� �������� ���̴�.
				meshRenderer->Render(immediateContext);
			}
		}
	}

	void Renderer::DrawLight()
	{

	}
}