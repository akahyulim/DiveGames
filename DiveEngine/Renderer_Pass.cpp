#include "Renderer.h"
#include "GBuffer.h"
#include "Log.h"
#include "GameObject.h"
#include "Camera.h"

namespace dive
{
	// Begin이라기보다는 Clear라는 이름이 더 어울릴 것 같다.
	void Renderer::BeginScene()
	{
		// RenderTargetView, DepthStencilView를 Clear 한다.
		// 대상은 Backbuffer(GraphicsDevice), GBuffer 둘 중 하나다.
		// 아니면 둘 다 해도 될 것 같다.
		// 현재 둘 다 개별 객체 Clear함수가 존재한다. 이를 호출하던지 아니면 View를 얻어와 Clear해야 한다.
		// 그런데 Clear color과 적용 여부가 Camera에 있다.
		// => 현재 BackBuffer를 Clear하는 PresentBegin이 Runtime::Render에서 호출된다.
		// => 이게 맞는 것 같기도 하고...
		// => 즉, Backbuffer는 따로 Clear하는게 나을 것 같다는 소리다.

		// Camera의 RenderTarget Clear는 개별 수행해야 할 것 같다.

		// 일단 메인 카메라의 존재 여부를 확인한다.
		// => 사실 Main Camera의 구분은 또 어떻게 해야할 지 생각해야 한다.
		// 없다면 특정 색상으로 Clear 후 리턴한다.
		// 메인 카메라가 있다면 Clear함수를 호출한다. 
	}

	// RenderTarget을 얻어와야 할 수 있다.
	void Renderer::PassGBuffer()
	{
		// 다중 카메라를 사용할 거라면
		// 이 곳에서 루프를 돌려야 한다.

		// 다중 카메라라면 MainCamera가 아니고 RenerTarget을 가질 경우 다시 ClearFlag를 수행해야 한다.
	}

	void Renderer::PassLighting()
	{
		// 다중 카메라를 사용할 거라면
		// 이 곳에서도 역시 루프를 돌려야 한다.

		// 카메라에 RenderTarget이 있다면 거기에다 그릴 것이다.
	}

	/*
	* ClearFlags에 적용할 값을 생각해보면
	* 결국 다수의 Camera들 중 대표하는 하나가 존재해야 할 것 같다.
	* 유니티에서는 그게 MainCamera인 것 같다.
	* 좀 더 알아보자.
	*/
	void Renderer::PassMultiCamTest(ID3D11RenderTargetView* pRTV)
	{
		// 다중 카메라를 사용할 거라면
		// 이 곳에서 루프를 돌려야 한다.

		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		// CameraClearFlags Test를 위해 Depth Only를 구현해보고 싶은데
		// 현재 DepthStencilView가 존재하지 않는다.

		// 일단 Legacy를 두 개 이상의 Camera로 그려보자.
		for (auto& pCamera : m_GameObjects[eObjectType::Camera])
		{
			if (m_GameObjects[eObjectType::Opaque].empty())
				return;

			auto pCameraCom = pCamera->GetComponent<Camera>();


			/*
			* 이렇게하면 바로 전 화면이 Clear되어 버린다.
			* 이게 Camera Clear Flags랑 관계가 있는건가?
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

			// Viewport 역시 일단 순서대로 적용한다.
			// 그런데... num을 설정하면 어떻게 되는거지?
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

				meshRenderer->Render(immediateContext);
			}
		}
	}
}