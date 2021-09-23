#include "Renderer.h"
#include "../DiveCore.h"
#include "../Log.h"
#include "../GameObject.h"
#include "../Camera.h"

namespace dive
{
	// 현재 테스트 용으로 한 곳에 몰아 넣었다.
	// 추후 세분화시킨 후 나누어야 한다.
	// 멀티 카메라를 테스트하던 구현이다.
	void Renderer::DrawScene()
	{
		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		DV_ASSERT(immediateContext != nullptr);

		// CameraClearFlags Test를 위해 Depth Only를 구현해보고 싶은데
		// 현재 DepthStencilView가 존재하지 않는다.

		// 일단 Legacy를 두 개 이상의 Camera로 그려보자.
		// => 이건 좀 특별한 케이스다. 원래는 Camera에 RenderTexture가 존재하므로 따로 그려야 한다.
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
			immediateContext->PSSetSamplers(0, 1, &m_PipelineStateLegacy.pSS);
			immediateContext->OMSetDepthStencilState(m_PipelineStateLegacy.pDSS, 1);
			immediateContext->RSSetState(m_PipelineStateLegacy.pRSS);

			// Viewport 역시 일단 순서대로 적용한다.
			// 그런데... num을 설정하면 어떻게 되는거지?
			//=> 테스트용임을 감안하자. Wicked는 Viewport를 RenderPath에서 이 함수 호출전에 Bind하였다. 물론 구현이 다를순 있다.
			immediateContext->RSSetViewports(1, pCameraCom->GetViewportPtr());

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
				meshRenderer->Render(immediateContext);
			}
		}
	}

	void Renderer::DrawLight()
	{

	}
}