#include "Renderer.h"
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
		mRenderTargetSize = DirectX::XMINT2(0, 0);
		// 이렇게 초기화하면 Sandbox에서 문제가 생긴다.
		mViewPort.Width = static_cast<float>(mRenderTargetSize.x);
		mViewPort.Height = static_cast<float>(mRenderTargetSize.y);
		//mViewPort.Width = (float)mGraphicsDevice->GetResolutionWidth();
		//mViewPort.Height = (float)mGraphicsDevice->GetResolutionHeight();
		mViewPort.MinDepth = 0.0f;
		mViewPort.MaxDepth = 1.0f;
		mViewPort.TopLeftX = 0.0f;
		mViewPort.TopLeftY = 0.0f;

		EVENT_SUBSCRIBE(eEventType::SceneResolve, EVENT_HANDLE(ObjectClassify));
	}

	Renderer::~Renderer()
	{
		// 리소스 제거
	}

	void Renderer::Initialize()
	{
		if (!mGraphicsDevice || !mGraphicsDevice->IsInitialized())
		{
			CORE_ERROR("Graphics Device가 생성되지 않아 초기화를 실행할 수 없습니다. 프로그램을 종료합니다.");
			PostQuitMessage(0);
		}

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
		if (mViewPort.Width != width || mViewPort.Height != height)
		{
			// 뭔가를 하는데...

			mViewPort.Width = width;
			mViewPort.Height = height;

			// 애초에 offset이 뭐냐...
		}
	}

	// 다시 한 번 말하지만 RenderTarget용 크기설정이다.
	void Renderer::SetResolution(unsigned int width, unsigned int height)
	{
		if (mRenderTargetSize.x == width && mRenderTargetSize.y == height)
			return;

		// 이외에도 크기가 맞는지 확인이 필요하다.

		mRenderTargetSize.x = width;
		mRenderTargetSize.y = height;

		createRenderTargets();

		CORE_TRACE("해상도 변경: {0:d}x{1:d}", width, height);
	}

	// 갱신만 한다. bind는 개별 path에서 한다.
	void Renderer::UpdateCB()
	{
		auto immediateContext = mGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		// CB Update
		if (mConstantBufferMatrix == nullptr)
			return;

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (FAILED(immediateContext->Map(mConstantBufferMatrix.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			CORE_ERROR("Constant Buffer Mapping에 실패하였습니다.");
			return;
		}

		MatrixBuffer* pBuffer = static_cast<MatrixBuffer*>(mappedResource.pData);
		
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
			pBuffer->view = XMMatrixTranspose(XMMatrixLookAtLH(positionVector, lookAtVector, upVector));
		}

		// Perspective Projection Matrix
		{
			float fieldOfView = 3.141592654f / 4.0f;
			float screenAspect = (float)mGraphicsDevice->GetResolutionWidth() / (float)mGraphicsDevice->GetResolutionHeight();
			pBuffer->proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f));
		}

		// Orthographic Projection Matrix
		{
			auto width = (float)mGraphicsDevice->GetResolutionWidth();
			auto height = (float)mGraphicsDevice->GetResolutionHeight();
			pBuffer->projOrthographic = XMMatrixTranspose(XMMatrixOrthographicLH(width, height, 0.1f, 1000.0f));
		}

		immediateContext->Unmap(static_cast<ID3D11Resource*>(mConstantBufferMatrix.Get()), 0);
	}

	void Renderer::DrawScene()
	{
		
	}

	void Renderer::DrawLegacy()
	{
		if (mGameObjects[eObjectType::Opaque].empty())
			return;

		auto immediateContext = mGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->IASetInputLayout(mPipelineStateLegacy.pIL);
		immediateContext->IASetPrimitiveTopology(mPipelineStateLegacy.primitiveTopology);
		immediateContext->VSSetShader(mPipelineStateLegacy.pVS, NULL, 0);
		immediateContext->PSSetShader(mPipelineStateLegacy.pPS, NULL, 0);
		immediateContext->PSSetSamplers(0, 1, &mPipelineStateLegacy.pSS);
		immediateContext->OMSetDepthStencilState(mPipelineStateLegacy.pDSS, 1);
		immediateContext->RSSetState(mPipelineStateLegacy.pRSS);
		immediateContext->VSSetConstantBuffers(0, 1, mConstantBufferMatrix.GetAddressOf());		// 이건 GameObject마다 가려내야 한다.
		immediateContext->RSSetViewports(1, &mViewPort);

		MeshRenderer* meshRenderer = nullptr;
		for (const auto& gameObject : mGameObjects[eObjectType::Opaque])
		{
			meshRenderer = gameObject->GetComponent<MeshRenderer>();
			
			ID3D11Buffer* vertexBuffer = meshRenderer->GetVertexBuffer();
			assert(vertexBuffer != nullptr);
			unsigned int stride = meshRenderer->GetVertexStride();
			unsigned int offset = 0;
			immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			ID3D11Buffer* indexBuffer = meshRenderer->GetIndexBuffer();
			assert(indexBuffer != nullptr);
			immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			immediateContext->DrawIndexed(meshRenderer->GetIndexCount(), 0, 0);

			CORE_TRACE("Legacy Object - {0:d}, {1:d}", stride, meshRenderer->GetIndexCount());
		}
	}

	// 함수이름이 자동으로 바뀌네...
	void Renderer::DrawText()
	{
		// GameObject에서 TextMesh만 뽑아서 호출하도록 바꿔야 한다.

		auto immediateContext = mGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->IASetInputLayout(mPipelineStateFont.pIL);
		immediateContext->IASetPrimitiveTopology(mPipelineStateFont.primitiveTopology);
		immediateContext->VSSetShader(mPipelineStateFont.pVS, NULL, 0);
		immediateContext->PSSetShader(mPipelineStateFont.pPS, NULL, 0);
		immediateContext->PSSetSamplers(0, 1, &mPipelineStateFont.pSS);	// 얘는 더블 포인터다...
		immediateContext->OMSetDepthStencilState(mPipelineStateFont.pDSS, 1);
		immediateContext->RSSetState(mPipelineStateFont.pRSS);

		immediateContext->VSSetConstantBuffers(0, 1, mConstantBufferMatrix.GetAddressOf());

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)mGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)mGraphicsDevice->GetResolutionHeight();
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
		mGameObjects.clear();
		// 카메라도 초기화?

		auto& gameObjects = Scene::GetGlobalScene().GetAllGameObjects();
		for (const auto& gameObject : gameObjects)
		{
			if (!gameObject->GetActive())
				continue;

			auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
			auto camera = gameObject->GetComponent<Camera>();
			auto light = gameObject->GetComponent<Light>();

			if (meshRenderer)
			{
				// Opaque와 Transparent의 구분은 Material을 이용한다.
				mGameObjects[eObjectType::Opaque].push_back(gameObject.get());
			}
			else if (camera)
			{
				mGameObjects[eObjectType::Camera].push_back(gameObject.get());
				// 카메라 선택... 메인 카메라인가?
			}
			else if (light)
			{
				mGameObjects[eObjectType::Light].push_back(gameObject.get());
			}
		}

		// Opaque와 Transparent는 근거리순으로 비교하여 재정렬한다.

		// 근데 업데이트 주기는 어떻게 되는걸까?
		// 그리고 Frustum Culling을 여기에서 적용하는건 무리인가?
	}

	void Renderer::SetGraphicsDevice(std::shared_ptr<GraphicsDevice> device)
	{
		mGraphicsDevice = device;

		assert(mGraphicsDevice);
	}	
}