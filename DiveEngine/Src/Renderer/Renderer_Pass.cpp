#include "Renderer.h"
#include "../Core/DiveCore.h"
#include "../Helper/Log.h"
#include "../Scene/GameObject.h"
#include "../Scene/Component/Camera.h"
#include "../Scene/Component/dvMeshRenderer.h"

// 일단 dvRenderer 구현은 나중으로 미루고
// 이 곳에서 Deferred Shader로 Lighting 까지 구현해보자.
namespace DiveEngine
{
	// Multi-Thread Render reference를 참고하자면
	// 이 곳에선 Static과 Dynamic Params만 설정하고
	// RenderScene() 같은 곳에서 Bind를 시킬 수 있다.
	// 그럴려면 Pass별 Params를 설정해 놓은 객체 혹은 변수가 필요하다.
	// static의 경우 RenderPath에서 RTV, DSV 등을 설정할 수 있을 것 같긴한데
	// Dynamic의 경우엔 뭘로 해야 할까?
	void Renderer::DrawScene()
	{
		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(immediateContext != nullptr);

		// CameraClearFlags Test를 위해 Depth Only를 구현해보고 싶은데
		// 현재 DepthStencilView가 존재하지 않는다.

		// 사실 MainCamera는 하나인게 맞다.
		// 나머지 Camera들은 RTV용이다.
		for (auto& pCamera : m_GameObjects[eObjectType::Camera])
		{
			if (m_GameObjects[eObjectType::Opaque].empty())
				return;

			auto pCameraCom = pCamera->GetComponent<Camera>();

			// PipelineState가 필요한지 의문이다.
			//immediateContext->RSSetViewports(1, pCameraCom->GetViewportPtr());
			immediateContext->IASetInputLayout(m_PipelineStateLegacy.pIL);
			immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//m_PipelineStateLegacy.primitiveTopology);
			immediateContext->VSSetShader(m_PipelineStateLegacy.pVS, NULL, 0);
			immediateContext->PSSetShader(m_PipelineStateLegacy.pPS, NULL, 0);
			//immediateContext->PSSetSamplers(0, 1, &m_PipelineStateLegacy.pSS);
			//immediateContext->PSSetSamplers(0, 1, m_pSamplerStates[(size_t)eSamplerState::Linear].GetAddressOf());
			//immediateContext->OMSetDepthStencilState(m_PipelineStateLegacy.pDSS, 1);
			immediateContext->RSSetState(m_PipelineStateLegacy.pRSS);

			// 결국 그려지는 GameObject의 Component들에서 Data를 Bind한 후 최종적으로 Draw를 호출한다.
			// 대상은 ConstantBuffer(변환 행렬, Material Data), Texture(Material, SRV), Buffer(Vertex, Index) 등이다.
			MeshRenderer* meshRenderer = nullptr;
			for (const auto& gameObject : m_GameObjects[eObjectType::Opaque])
			{
				if (Transform* transform = gameObject->GetTransform())
				{
					// view proj 역시 camera로부터 직접 가져왔다.
					m_BufferObjectCPU.SetWorldMatrix(transform->GetMatrix());
					m_BufferObjectCPU.SetWorldViewProjectionMatrix(m_BufferObjectCPU.GetWorldMatrix() * pCameraCom->GetViewProjectionMatrix());

					//==========================================================================//
					// Constant Buffer Test														//
					// 1. 스파르탄은 함수를 사용해 CPU 데이터를 GPU에 map / unmap 했다.			//
					// 2. DirectX의 행렬과 HLSL의 행렬 방향이 다르기때문에 전치해 주어야 한다.	//
					// 현재 XMFLOAT4X4로 저장했기에 좀 더 복잡해졌다.							//
					//==========================================================================//
					DirectX::XMMATRIX world = XMMatrixTranspose(m_BufferObjectCPU.GetWorldMatrix());
					DirectX::XMMATRIX wvp = XMMatrixTranspose(m_BufferObjectCPU.GetWorldViewProjectionMatrix());

					// 이건 오브젝트 버퍼다. UpdateCB랑은 상관없다.
					BufferObject* pData = static_cast<BufferObject*>(m_pBufferObjectGPU->Map());
					DirectX::XMStoreFloat4x4(&pData->world, world);
					DirectX::XMStoreFloat4x4(&pData->wvp, wvp);
					assert(m_pBufferObjectGPU->Unmap());

					// 사실 이 역시 Transform에서 Constant Buffer를 만들어 놓고 직접 Bind하는 방법이 있긴 하다.
					ID3D11Buffer* buffer = m_pBufferObjectGPU->GetBuffer();
					immediateContext->VSSetConstantBuffers(0, 1, &buffer);
				}

				meshRenderer = gameObject->GetComponent<MeshRenderer>();
				if (meshRenderer)
				{
					meshRenderer->Render(immediateContext);
				}

				// base인 MeshRenderer로 받아서 Draw를 호출할 순 없을까?
				// Shader는 어차피 Material에서 획득할테니 말이다.
				dvMeshRenderer* pDmr = gameObject->GetComponent<StaticMeshRenderer>();
				if (pDmr)
				{
					pDmr->Draw(immediateContext);
				}
			}
		}
	}

	// 나중에 DrawScene을 지우고 이름을 바꾸자.
	void Renderer::DrawGBuffer()
	{
		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(immediateContext != nullptr);

		// m_GameObjects가 갱신되지 않는 경우가 발생했었다.
		// 고쳐진 이유를 모르겠다.
		for (auto& pCamera : m_GameObjects[eObjectType::Camera])
		{
			if (m_GameObjects[eObjectType::Opaque].empty())
				return;

			auto pCameraCom = pCamera->GetComponent<Camera>();

	
			// 몇개는 GET 함수가 있다...
			// 그런데 더블 포인터 타입때문에 직접 가져오는게 편하다.
			immediateContext->VSSetShader((ID3D11VertexShader*)m_pShaders[SHADERTYPES::VSTYPE_DEFERRED_SHADING].Get(), NULL, 0);
			immediateContext->PSSetShader((ID3D11PixelShader*)m_pShaders[SHADERTYPES::PSTYPE_DEFERRED_SHADING].Get(), NULL, 0);
			immediateContext->IASetInputLayout(m_pInputLayouts[ILTYPES::ILTYPE_POS_TEX_NOR_TAN].Get());
			// 시작 슬롯, 개수, 포인터로 묶에서 보낼 수 있다.
			immediateContext->PSSetSamplers(0, 1, m_pSamplerStates[(size_t)eSamplerState::Linear].GetAddressOf());
			immediateContext->OMSetDepthStencilState(m_pDepthStencilStates[(size_t)eDepthStencilState::Default].Get(), 1);
			immediateContext->RSSetState(m_pRasterizerStates[(size_t)eRasterizerState::Cullback_Solid].Get());
			immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// DeferredShading.hlsl을 적용하려면 Specular에 대한 ConstantBuffer도 만들어서 전달해야 한다.

			MeshRenderer* meshRenderer = nullptr;
			for (const auto& gameObject : m_GameObjects[eObjectType::Opaque])
			{
				if (Transform* transform = gameObject->GetTransform())
				{
					// view proj 역시 camera로부터 직접 가져왔다.
					m_BufferObjectCPU.SetWorldMatrix(transform->GetMatrix());
					m_BufferObjectCPU.SetWorldViewProjectionMatrix(m_BufferObjectCPU.GetWorldMatrix() * pCameraCom->GetViewProjectionMatrix());

					//==========================================================================//
					// Constant Buffer Test														//
					// 1. 스파르탄은 함수를 사용해 CPU 데이터를 GPU에 map / unmap 했다.			//
					// 2. DirectX의 행렬과 HLSL의 행렬 방향이 다르기때문에 전치해 주어야 한다.	//
					// 현재 XMFLOAT4X4로 저장했기에 좀 더 복잡해졌다.							//
					//==========================================================================//
					DirectX::XMMATRIX world = XMMatrixTranspose(m_BufferObjectCPU.GetWorldMatrix());
					DirectX::XMMATRIX wvp = XMMatrixTranspose(m_BufferObjectCPU.GetWorldViewProjectionMatrix());

					// 현재 바로 위 DrawScene()과 동일한 코드이지만
					// 이 cb는 DeferredShading에서 사용되어야 하는 cb이다.
					// 그 곳에선 cb의 구성이 wvp, world로 되어 있으므로, 순서상 문제가 발생할 수 있다.
					// 물론 shader쪽 구성을 바꾸는 것이 훨씬 간단하겠지만, 그 곳은 또 offset 때문에 문제가 발생할 수 있다.
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

		// 여기서도 Draw Call 발생
		pImmediateContext->Draw(4, 0);

		// Cleanup
		ID3D11ShaderResourceView* arrRV[1] = { NULL };
		pImmediateContext->PSSetShaderResources(4, 1, arrRV);
		pImmediateContext->VSSetShader(NULL, NULL, 0);
		pImmediateContext->PSSetShader(NULL, NULL, 0);
		pImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}