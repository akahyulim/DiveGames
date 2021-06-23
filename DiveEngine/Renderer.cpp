#include "Renderer.h"
#include "Scene.h"
#include "Mesh.h"
#include "Log.h"
#include "TextMesh.h"
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
		auto pImmediateContext = mGraphicsDevice->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		// CB Update
		if (mConstantBufferMatrix == nullptr)
			return;

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (FAILED(pImmediateContext->Map(mConstantBufferMatrix.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			CORE_ERROR("Constant Buffer Mapping에 실패하였습니다.");
			return;
		}

		MatrixBuffer* pBuffer = static_cast<MatrixBuffer*>(mappedResource.pData);
		
		// World Matrix
		pBuffer->world = XMMatrixTranspose(XMMatrixIdentity());

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

		pImmediateContext->Unmap(static_cast<ID3D11Resource*>(mConstantBufferMatrix.Get()), 0);
	}

	void Renderer::DrawScene()
	{
		
	}

	// 이걸 적용하려면 IL가 맞아야 한다.
	void Renderer::DrawColor()
	{
		// 이건 임시다. Visibility 등을 통해 얻어야 한다.
		Mesh* mesh = Scene::GetGlobalScene().GetMesh();
		if (!mesh)
			return;

		auto pImmediateContext = mGraphicsDevice->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		pImmediateContext->IASetInputLayout(mPipelineStateColor.pIL);
		pImmediateContext->IASetPrimitiveTopology(mPipelineStateColor.primitiveTopology);
		pImmediateContext->VSSetShader(mPipelineStateColor.pVS, NULL, 0);
		pImmediateContext->PSSetShader(mPipelineStateColor.pPS, NULL, 0);
		pImmediateContext->OMSetDepthStencilState(mPipelineStateColor.pDSS, 1);
		pImmediateContext->RSSetState(mPipelineStateColor.pRSS);

		pImmediateContext->VSSetConstantBuffers(0, 1, mConstantBufferMatrix.GetAddressOf());
		pImmediateContext->RSSetViewports(1, &mViewPort);

		ID3D11Buffer* vbs[] =
		{
			mesh->m_pVBPosition.Get(),
			mesh->m_pVBColor.Get()
		};

		UINT strides[] =
		{
			sizeof(XMFLOAT3),
			sizeof(XMFLOAT4)
		};

		unsigned int offsets[] =
		{
			0,
			0
		};

		pImmediateContext->IASetVertexBuffers(0, arraysize(vbs), vbs, strides, offsets);
		pImmediateContext->IASetIndexBuffer(mesh->m_pIB.Get(), DXGI_FORMAT_R32_UINT, 0);

		pImmediateContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}

	void Renderer::DrawTexturing()
	{
		auto pImmediateContext = mGraphicsDevice->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		pImmediateContext->IASetInputLayout(mPipelineStateTexturing.pIL);
		pImmediateContext->IASetPrimitiveTopology(mPipelineStateTexturing.primitiveTopology);
		pImmediateContext->VSSetShader(mPipelineStateTexturing.pVS, NULL, 0);
		pImmediateContext->PSSetShader(mPipelineStateTexturing.pPS, NULL, 0);
		pImmediateContext->PSSetSamplers(0, 1, &mPipelineStateTexturing.pSS);	// 얘는 더블 포인터다...
		pImmediateContext->OMSetDepthStencilState(mPipelineStateTexturing.pDSS, 1);
		pImmediateContext->RSSetState(mPipelineStateTexturing.pRSS);

		pImmediateContext->VSSetConstantBuffers(0, 1, mConstantBufferMatrix.GetAddressOf());


		// 임시
		// 누가 가져야 할까?
		D3D11_VIEWPORT viewport;
		viewport.Width = (float)mGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)mGraphicsDevice->GetResolutionHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		pImmediateContext->RSSetViewports(1, &viewport);

		// 이건 임시다. Visibility 등을 통해 얻어야 한다.
		Mesh* mesh = Scene::GetGlobalScene().GetMesh();

		ID3D11Buffer* vbs[] =
		{
			mesh->m_pVBPosition.Get(),
			mesh->m_pVBUv.Get()
		};

		UINT strides[] =
		{
			sizeof(XMFLOAT3),
			sizeof(XMFLOAT2)
		};

		unsigned int offsets[] =
		{
			0,
			0
		};

		// 위치와 방법이 마음에 들지 않는다.
		auto pSRV = mTexture->GetShaderResourceView();
		//auto pSRV = mCpuTexture->GetShaderResourceView();
		//auto pSRV = mDvFont->GetAtlas()->GetShaderResourceView();
		pImmediateContext->PSSetShaderResources(0, 1, &pSRV);

		pImmediateContext->IASetVertexBuffers(0, arraysize(vbs), vbs, strides, offsets);
		pImmediateContext->IASetIndexBuffer(mesh->m_pIB.Get(), DXGI_FORMAT_R32_UINT, 0);

		pImmediateContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}

	// 함수이름이 자동으로 바뀌네...
	void Renderer::DrawText()
	{
		// GameObject에서 TextMesh만 뽑아서 호출하도록 바꿔야 한다.

		auto pImmediateContext = mGraphicsDevice->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		pImmediateContext->IASetInputLayout(mPipelineStateFont.pIL);
		pImmediateContext->IASetPrimitiveTopology(mPipelineStateFont.primitiveTopology);
		pImmediateContext->VSSetShader(mPipelineStateFont.pVS, NULL, 0);
		pImmediateContext->PSSetShader(mPipelineStateFont.pPS, NULL, 0);
		pImmediateContext->PSSetSamplers(0, 1, &mPipelineStateFont.pSS);	// 얘는 더블 포인터다...
		pImmediateContext->OMSetDepthStencilState(mPipelineStateFont.pDSS, 1);
		pImmediateContext->RSSetState(mPipelineStateFont.pRSS);

		pImmediateContext->VSSetConstantBuffers(0, 1, mConstantBufferMatrix.GetAddressOf());

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)mGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)mGraphicsDevice->GetResolutionHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		pImmediateContext->RSSetViewports(1, &viewport);

		//auto pSRV = m_pTextMesh->GetAtlas();
		//pImmediateContext->PSSetShaderResources(0, 1, &pSRV);

		//auto pVB = m_pTextMesh->GetVertexBuffer();
		///unsigned int stride = m_pTextMesh->GetStride();
		//unsigned int offset = 0;
		//pImmediateContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		//pImmediateContext->IASetIndexBuffer(m_pTextMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		//pImmediateContext->DrawIndexed(m_pTextMesh->GetIndexCount(), 0, 0);
	}

	// 스파르탄은 이벤트 callback 함수다.
	// 그런데 현재 내 이벤트 시스템에선 Variant를 매개변수로 받을 수 없다.
	void Renderer::ObjectClassify()
	{
		// 모든 GameObject를 순회하면서 Component 여부로 나누어 저장한다.
		// Opaque와 Transparent의 구분은 Material을 이용한다.

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