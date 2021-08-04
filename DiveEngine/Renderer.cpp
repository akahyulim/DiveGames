#include "Renderer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Light.h"
#include "GameObject.h"
#include "Log.h"
#include "TextMesh.h"
#include "Event.h"
#include <assert.h>

namespace dive
{
	Renderer::Renderer()
	{
		m_RenderTargetSize = DirectX::XMINT2(0, 0);
		// 이렇게 초기화하면 Sandbox에서 문제가 생긴다.
		m_ViewPort.Width = static_cast<float>(m_RenderTargetSize.x);
		m_ViewPort.Height = static_cast<float>(m_RenderTargetSize.y);
		//m_ViewPort.Width = (float)m_pGraphicsDevice->GetResolutionWidth();
		//m_ViewPort.Height = (float)m_pGraphicsDevice->GetResolutionHeight();
		m_ViewPort.MinDepth = 0.0f;
		m_ViewPort.MaxDepth = 1.0f;
		m_ViewPort.TopLeftX = 0.0f;
		m_ViewPort.TopLeftY = 0.0f;

		EVENT_SUBSCRIBE(eEventType::SceneResolve, EVENT_HANDLE(ObjectClassify));
	}

	Renderer::~Renderer()
	{
		// 리소스 제거
	}

	void Renderer::Initialize()
	{
		if (!m_pGraphicsDevice || !m_pGraphicsDevice->IsInitialized())
		{
			CORE_ERROR("Graphics Device가 생성되지 않아 초기화를 실행할 수 없습니다. 프로그램을 종료합니다.");
			PostQuitMessage(0);
		}

		// 형태가 좀... 일관성이 없다.
		m_pGBuffer = new GBuffer(m_pGraphicsDevice->GetDevice());
		// 크기가 이러면 안된다. 일반적으로 맞지만 Editor의 Scene과는 다르다.
		m_pGBuffer->Initialize(m_pGraphicsDevice->GetResolutionHeight(), m_pGraphicsDevice->GetResolutionHeight());

		// states
		createDepthStencilStates();
		createRasterizerStates();
		createSamplerStates();

		createConstantBuffers();
		//createTextures();				// 현재는 Sandbox 테스트용, Spartan은 Editor에서 사용하는 Texture 생성용
		createRenderTargets();		// 여러 RenderTexture들을 만든다.
		createShaders();

		createFonts();

		createPipelineStates();	// 가장 마지막이어야 한다.
		
		CORE_TRACE("Renderer 초기화에 성공하였습니다.");
	}

	void Renderer::SetViewport(float width, float height, float offsetX, float offsetY)
	{
		if (m_ViewPort.Width != width || m_ViewPort.Height != height)
		{
			// 뭔가를 하는데...

			m_ViewPort.Width = width;
			m_ViewPort.Height = height;

			// 애초에 offset이 뭐냐...
		}
	}

	// 다시 한 번 말하지만 RenderTarget용 크기설정이다.
	void Renderer::SetResolution(unsigned int width, unsigned int height)
	{
		if (m_RenderTargetSize.x == width && m_RenderTargetSize.y == height)
			return;

		// 이외에도 크기가 맞는지 확인이 필요하다.

		m_RenderTargetSize.x = width;
		m_RenderTargetSize.y = height;

		createRenderTargets();

		CORE_TRACE("해상도 변경: {0:d}x{1:d}", width, height);
	}

	//======================================================================================//
	// 갱신만 한다. bind는 개별 path에서 한다.												//
	// 현재 wciked와 spartan의 구현이 꼬였다.												//
	// Main Camera를 통해 View, Projection을 이용한 행렬을 계산해							//
	// 매 프레임 갱신하는 Constant Buffer의 데이터를 저장(CPU)한다.							//
	// 현재 구버전 Constant Buffer에 하드 코딩된 View, Projection이 사용되어				//
	// 복잡하다.																			//
	//======================================================================================//
	void Renderer::UpdateCB()
	{
		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		if (m_pBufferFrame == nullptr)
			return;

		MatrixBuffer* pBuffer = static_cast<MatrixBuffer*>(m_pBufferFrame->Map());

		// World Matrix
		// 이건 개별 GameObject의 Transform으로부터 가져온다.
		pBuffer->world = XMMatrixTranspose(XMMatrixIdentity());

		// View와 Projection은 Camera로부터 가져온다.
		// View Matrix
		{
			XMFLOAT3 up, position, lookAt;
			XMVECTOR upVector, positionVector, lookAtVector;
			float yaw, pitch, roll;
			XMMATRIX rotationMatrix;

			up.x = 0.0f;
			up.y = 1.0f;
			up.z = 0.0f;

			upVector = XMLoadFloat3(&up);

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
			pitch = 0.0f * 0.0174532925f;
			yaw = 0.0f * 0.0174532925f;
			roll = 0.0f * 0.0174532925f;

			// Create the rotation matrix from the yaw, pitch, and roll values.
			rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

			// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
			lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
			upVector = XMVector3TransformCoord(upVector, rotationMatrix);

			// Translate the rotated camera position to the location of the viewer.
			lookAtVector = XMVectorAdd(positionVector, lookAtVector);

			auto viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
			pBuffer->view = XMMatrixTranspose(viewMatrix);
			m_BufferFrameCPU.SetViewMatrix(viewMatrix);
		}

		// Perspective Projection Matrix
		{
			float fieldOfView = 3.141592654f / 4.0f;
			float screenAspect = (float)m_pGraphicsDevice->GetResolutionWidth() / (float)m_pGraphicsDevice->GetResolutionHeight();
			
			auto projMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f);
			pBuffer->proj = XMMatrixTranspose(projMatrix);
			m_BufferFrameCPU.SetPerspectiveProjectionMatrix(projMatrix);
			auto view_proj = m_BufferFrameCPU.GetViewMatrix() * m_BufferFrameCPU.GetPerspectiveProjectionMatrix();
			m_BufferFrameCPU.SetViewProjectionMatrix(view_proj);
		}

		// Orthographic Projection Matrix
		{
			auto width = (float)m_pGraphicsDevice->GetResolutionWidth();
			auto height = (float)m_pGraphicsDevice->GetResolutionHeight();
			auto orthoProjMatrix = XMMatrixTranspose(XMMatrixOrthographicLH(width, height, 0.1f, 1000.0f));
			pBuffer->projOrthographic = orthoProjMatrix;
			m_BufferFrameCPU.SetOrthoProjectionMatrix(orthoProjMatrix);
		}

		//immediateContext->Unmap(static_cast<ID3D11Resource*>(mConstantBufferMatrix.Get()), 0);
		m_pBufferFrame->Unmap();
	}

	void Renderer::DrawScene()
	{
		
	}

	void Renderer::DrawLegacy()
	{
		if (m_GameObjects[eObjectType::Opaque].empty())
			return;

		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->IASetInputLayout(m_PipelineStateLegacy.pIL);
		immediateContext->IASetPrimitiveTopology(m_PipelineStateLegacy.primitiveTopology);
		immediateContext->VSSetShader(m_PipelineStateLegacy.pVS, NULL, 0);
		immediateContext->PSSetShader(m_PipelineStateLegacy.pPS, NULL, 0);
		immediateContext->PSSetSamplers(0, 1, &m_PipelineStateLegacy.pSS);
		immediateContext->OMSetDepthStencilState(m_PipelineStateLegacy.pDSS, 1);
		immediateContext->RSSetState(m_PipelineStateLegacy.pRSS);
		immediateContext->RSSetViewports(1, &m_ViewPort);


		MeshRenderer* meshRenderer = nullptr;
		for (const auto& gameObject : m_GameObjects[eObjectType::Opaque])
		{
			if (Transform* transform = gameObject->GetTransform())
			{
				m_BufferObjectCPU.SetWorldMatrix(transform->GetMatrix());
				m_BufferObjectCPU.SetWorldViewProjectionMatrix(m_BufferObjectCPU.GetWorldMatrix() * m_BufferFrameCPU.GetViewProjectionMatrix());

				//==========================================================================//
				// Constant Buffer Test														//
				// 1. 스파르탄은 함수를 사용해 CPU 데이터를 GPU에 map / unmap 했다.			//
				// 2. DirectX의 행렬과 HLSL의 행렬 방향이 다르기때문에 전치해 주어야 한다.	//
				// 현재 XMFLOAT4X4로 저장했기에 좀 더 복잡해졌다.							//
				//==========================================================================//
				DirectX::XMMATRIX world = XMMatrixTranspose(m_BufferObjectCPU.GetWorldMatrix());
				DirectX::XMMATRIX wvp = XMMatrixTranspose(m_BufferObjectCPU.GetWorldViewProjectionMatrix());

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

			/*
			ID3D11Buffer* vertexBuffer = meshRenderer->GetVertexBuffer();
			assert(vertexBuffer != nullptr);
			unsigned int stride = meshRenderer->GetVertexStride();
			unsigned int offset = 0;
			immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			ID3D11Buffer* indexBuffer = meshRenderer->GetIndexBuffer();
			assert(indexBuffer != nullptr);
			immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			immediateContext->DrawIndexed(meshRenderer->GetIndexCount(), 0, 0);
			*/
			//CORE_TRACE("Legacy Object - {0:d}, {1:d}", stride, meshRenderer->GetIndexCount());
		}
	}

	// 함수이름이 자동으로 바뀌네...
	void Renderer::DrawText()
	{
		// GameObject에서 TextMesh만 뽑아서 호출하도록 바꿔야 한다.

		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->IASetInputLayout(m_PipelineStateFont.pIL);
		immediateContext->IASetPrimitiveTopology(m_PipelineStateFont.primitiveTopology);
		immediateContext->VSSetShader(m_PipelineStateFont.pVS, NULL, 0);
		immediateContext->PSSetShader(m_PipelineStateFont.pPS, NULL, 0);
		immediateContext->PSSetSamplers(0, 1, &m_PipelineStateFont.pSS);	// 얘는 더블 포인터다...
		immediateContext->OMSetDepthStencilState(m_PipelineStateFont.pDSS, 1);
		immediateContext->RSSetState(m_PipelineStateFont.pRSS);

		ID3D11Buffer* buffer = m_pBufferFrame->GetBuffer();
		immediateContext->VSSetConstantBuffers(0, 1, &buffer);

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)m_pGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)m_pGraphicsDevice->GetResolutionHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		immediateContext->RSSetViewports(1, &viewport);

		//auto pSRV = m_pTextMesh->GetAtlas();
		//immediateContext->PSSetShaderResources(0, 1, &pSRV);

		//auto pVB = m_pTextMesh->GetVertexBuffer();
		///unsigned int stride = m_pTextMesh->GetStride();
		//unsigned int offset = 0;
		//immediateContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		//immediateContext->IASetIndexBuffer(m_pTextMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		//immediateContext->DrawIndexed(m_pTextMesh->GetIndexCount(), 0, 0);
	}

	// =========================================================================//
	// Scene에서 GameObjects가 갱신될 때 마다 호출된다.							//
	// 갱신의 시점은 Scene과 GameObject에서 이벤트를 발생시키는 것인데			//
	// 아직 분석이 덜 되었다.													//
	//==========================================================================//
	void Renderer::ObjectClassify()
	{
		m_GameObjects.clear();
		// 카메라도 초기화?

		// 결국 여기까지 왔다.
		auto& gameObjects = SceneManager::GetInstance().GetActiveScene()->GetAllGameObjects();
		for (const auto& gameObject : gameObjects)
		{
			if (!gameObject->IsActive())
				continue;

			auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
			auto camera = gameObject->GetComponent<Camera>();
			auto light = gameObject->GetComponent<Light>();

			if (meshRenderer)
			{
				// Opaque와 Transparent의 구분은 Material을 이용한다.
				m_GameObjects[eObjectType::Opaque].push_back(gameObject);
			}
			else if (camera)
			{
				m_GameObjects[eObjectType::Camera].push_back(gameObject);
				// 카메라 선택... 메인 카메라인가?
			}
			else if (light)
			{
				m_GameObjects[eObjectType::Light].push_back(gameObject);
			}
		}

		// Opaque와 Transparent는 근거리순으로 비교하여 재정렬한다.

		// 근데 업데이트 주기는 어떻게 되는걸까?
		// 그리고 Frustum Culling을 여기에서 적용하는건 무리인가?
	}

	void Renderer::SetGraphicsDevice(std::shared_ptr<GraphicsDevice> pDevice)
	{
		m_pGraphicsDevice = pDevice;

		assert(m_pGraphicsDevice);
	}	
}