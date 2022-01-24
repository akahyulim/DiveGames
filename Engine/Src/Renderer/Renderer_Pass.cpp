#include "Renderer.h"
#include "../Core/DiveCore.h"
#include "../Helper/Log.h"
#include "../Scene/GameObject.h"
#include "../Scene/Component/Camera.h"
#include "../Scene/Component/dvMeshRenderer.h"

// �ϴ� dvRenderer ������ �������� �̷��
// �� ������ Deferred Shader�� Lighting ���� �����غ���.
namespace DiveEngine
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

		// ��� MainCamera�� �ϳ��ΰ� �´�.
		// ������ Camera���� RTV���̴�.
		for (auto& pCamera : m_GameObjects[eObjectType::Camera])
		{
			if (m_GameObjects[eObjectType::Opaque].empty())
				return;

			auto pCameraCom = pCamera->GetComponent<Camera>();

			// PipelineState�� �ʿ����� �ǹ��̴�.
			//immediateContext->RSSetViewports(1, pCameraCom->GetViewportPtr());
			immediateContext->IASetInputLayout(m_PipelineStateLegacy.pIL);
			immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//m_PipelineStateLegacy.primitiveTopology);
			immediateContext->VSSetShader(m_PipelineStateLegacy.pVS, NULL, 0);
			immediateContext->PSSetShader(m_PipelineStateLegacy.pPS, NULL, 0);
			//immediateContext->PSSetSamplers(0, 1, &m_PipelineStateLegacy.pSS);
			//immediateContext->PSSetSamplers(0, 1, m_pSamplerStates[(size_t)eSamplerState::Linear].GetAddressOf());
			//immediateContext->OMSetDepthStencilState(m_PipelineStateLegacy.pDSS, 1);
			immediateContext->RSSetState(m_PipelineStateLegacy.pRSS);

			// �ᱹ �׷����� GameObject�� Component�鿡�� Data�� Bind�� �� ���������� Draw�� ȣ���Ѵ�.
			// ����� ConstantBuffer(��ȯ ���, Material Data), Texture(Material, SRV), Buffer(Vertex, Index) ���̴�.
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

					// ��� �� ���� Transform���� Constant Buffer�� ����� ���� ���� Bind�ϴ� ����� �ֱ� �ϴ�.
					ID3D11Buffer* buffer = m_pBufferObjectGPU->GetBuffer();
					immediateContext->VSSetConstantBuffers(0, 1, &buffer);
				}

				meshRenderer = gameObject->GetComponent<MeshRenderer>();
				if (meshRenderer)
				{
					meshRenderer->Render(immediateContext);
				}

				// base�� MeshRenderer�� �޾Ƽ� Draw�� ȣ���� �� ������?
				// Shader�� ������ Material���� ȹ�����״� ���̴�.
				dvMeshRenderer* pDmr = gameObject->GetComponent<StaticMeshRenderer>();
				if (pDmr)
				{
					pDmr->Draw(immediateContext);
				}
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

			// DeferredShading.hlsl�� �����Ϸ��� Specular�� ���� ConstantBuffer�� ���� �����ؾ� �Ѵ�.

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

				meshRenderer->Render(immediateContext);
			}
		}
	}

	void Renderer::DrawLight()
	{

	}

	void Renderer::DrawCompose()
	{
		auto pImmediateContext = m_pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(pImmediateContext != nullptr);

		pImmediateContext->PSSetSamplers(0, 1, m_pSamplerStates[(size_t)eSamplerState::Linear].GetAddressOf());

		pImmediateContext->IASetInputLayout(NULL);
		pImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
		pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// Set the shaders
		pImmediateContext->VSSetShader((ID3D11VertexShader*)m_pShaders[SHADERTYPES::VSTYPE_COMPOSE].Get(), NULL, 0);
		pImmediateContext->GSSetShader(NULL, NULL, 0);
		pImmediateContext->PSSetShader((ID3D11PixelShader*)m_pShaders[SHADERTYPES::PSTYPE_COMPOSE].Get(), NULL, 0);

		// ���⼭�� Draw Call �߻�
		pImmediateContext->Draw(4, 0);

		// Cleanup
		ID3D11ShaderResourceView* arrRV[1] = { NULL };
		pImmediateContext->PSSetShaderResources(4, 1, arrRV);
		pImmediateContext->VSSetShader(NULL, NULL, 0);
		pImmediateContext->PSSetShader(NULL, NULL, 0);
		pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}