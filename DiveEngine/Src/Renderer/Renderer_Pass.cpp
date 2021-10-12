#include "Renderer.h"
#include "../DiveCore.h"
#include "../Log.h"
#include "../GameObject.h"
#include "../Camera.h"

// 일단 dvRenderer 구현은 나중으로 미루고
// 이 곳에서 Deferred Shader로 Lighting 까지 구현해보자.
namespace dive
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

		// 유니티의 다중 카메라 예제를 따라해본 구현이다.
		// 이를 그대로 적용하려면 적어도 MainCamera 정도로 제한이 필요하다.
		// 그리고 Viewport 설정에 필요한 작업도 내부에서 해 주어야 한다.
		for (auto& pCamera : m_GameObjects[eObjectType::Camera])
		{
			if (m_GameObjects[eObjectType::Opaque].empty())
				return;

			auto pCameraCom = pCamera->GetComponent<Camera>();

			// PipelineState를 만들어 놓고 또 이렇게 Bind하는게 맞나 싶다.
			// 그런데 다시보니 개별 추가 데이터가 필요하긴 하다.
			immediateContext->IASetInputLayout(m_PipelineStateLegacy.pIL);
			immediateContext->IASetPrimitiveTopology(m_PipelineStateLegacy.primitiveTopology);
			immediateContext->VSSetShader(m_PipelineStateLegacy.pVS, NULL, 0);
			immediateContext->PSSetShader(m_PipelineStateLegacy.pPS, NULL, 0);
			//immediateContext->PSSetSamplers(0, 1, &m_PipelineStateLegacy.pSS);
			immediateContext->PSSetSamplers(0, 1, m_pSamplerStates[(size_t)eSamplerState::Linear].GetAddressOf());
			immediateContext->OMSetDepthStencilState(m_PipelineStateLegacy.pDSS, 1);
			immediateContext->RSSetState(m_PipelineStateLegacy.pRSS);

			// Viewport 역시 일단 순서대로 적용한다.
			// 그런데... num을 설정하면 어떻게 되는거지?
			// 만약 num을 MainCamera 개수만큼, Viewport 포인터를 배열로 전달할 수 있다면
			// 카메라 개수만큼 루프를 돌리지 않아도 될 것이다.
			// 다만 이 경우 변환 행렬을 따로 전달할 수 있어야 한다.
			//immediateContext->RSSetViewports(1, pCameraCom->GetViewportPtr());

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

					ID3D11Buffer* buffer = m_pBufferObjectGPU->GetBuffer();
					immediateContext->VSSetConstantBuffers(0, 1, &buffer);
				}

				meshRenderer = gameObject->GetComponent<MeshRenderer>();
				if (!meshRenderer)
					continue;

				// 이건 다시 Draw로 바꿔야 할 것 같다.
				// 그러면 AssetData Bind가 추가돼 더 복잡해질 것이다.
				meshRenderer->Render(immediateContext);
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

				// 이건 다시 Draw로 바꿔야 할 것 같다.
				// 그러면 AssetData Bind가 추가돼 더 복잡해질 것이다.
				meshRenderer->Render(immediateContext);
			}
		}
	}

	void Renderer::DrawLight()
	{

	}
}