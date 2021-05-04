#include "Renderer.h"
#include "Scene.h"
#include "Mesh.h"
#include "Log.h"
#include <assert.h>

namespace Dive
{
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

		// states
		createDepthStencilStates();
		createRasterizerStates();
		createSamplerStates();

		createConstantBuffers();
		createTextures();
		createRenderTargetViews();
		createShaders();

		createFonts();

		createPipelineStates();	// 가장 마지막이어야 한다.
		
		CORE_TRACE("Renderer 초기화에 성공하였습니다.");
	}

	// 갱신만 한다. bind는 개별 path에서 한다.
	void Renderer::UpdateCB()
	{
		auto pImmediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		// CB Update
		if (m_pCBMatrix == nullptr)
			return;

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (FAILED(pImmediateContext->Map(m_pCBMatrix.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
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
			float screenAspect = (float)m_pGraphicsDevice->GetResolutionWidth() / (float)m_pGraphicsDevice->GetResolutionHeight();
			pBuffer->proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f));
		}

		// Orthographic Projection Matrix
		{
			auto width = (float)m_pGraphicsDevice->GetResolutionWidth();
			auto height = (float)m_pGraphicsDevice->GetResolutionHeight();
			pBuffer->projOrthographic = XMMatrixTranspose(XMMatrixOrthographicLH(width, height, 0.1f, 1000.0f));
		}

		pImmediateContext->Unmap(static_cast<ID3D11Resource*>(m_pCBMatrix.Get()), 0);
	}

	void Renderer::DrawScene()
	{
		
	}

	// 이걸 적용하려면 IL가 맞아야 한다.
	void Renderer::DrawColor()
	{
		auto pImmediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		pImmediateContext->IASetInputLayout(m_pipelineStateColor.pIL);
		pImmediateContext->IASetPrimitiveTopology(m_pipelineStateColor.primitiveTopology);
		pImmediateContext->VSSetShader(m_pipelineStateColor.pVS, NULL, 0);
		pImmediateContext->PSSetShader(m_pipelineStateColor.pPS, NULL, 0);
		pImmediateContext->OMSetDepthStencilState(m_pipelineStateColor.pDSS, 1);
		pImmediateContext->RSSetState(m_pipelineStateColor.pRSS);

		pImmediateContext->VSSetConstantBuffers(0, 1, m_pCBMatrix.GetAddressOf());


		// 임시
		// 누가 가져야 할까?
		D3D11_VIEWPORT viewport;
		viewport.Width = (float)m_pGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)m_pGraphicsDevice->GetResolutionHeight();
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
		auto pImmediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		pImmediateContext->IASetInputLayout(m_pipelineStateTexturing.pIL);
		pImmediateContext->IASetPrimitiveTopology(m_pipelineStateTexturing.primitiveTopology);
		pImmediateContext->VSSetShader(m_pipelineStateTexturing.pVS, NULL, 0);
		pImmediateContext->PSSetShader(m_pipelineStateTexturing.pPS, NULL, 0);
		pImmediateContext->PSSetSamplers(0, 1, &m_pipelineStateTexturing.pSS);	// 얘는 더블 포인터다...
		pImmediateContext->OMSetDepthStencilState(m_pipelineStateTexturing.pDSS, 1);
		pImmediateContext->RSSetState(m_pipelineStateTexturing.pRSS);

		pImmediateContext->VSSetConstantBuffers(0, 1, m_pCBMatrix.GetAddressOf());


		// 임시
		// 누가 가져야 할까?
		D3D11_VIEWPORT viewport;
		viewport.Width = (float)m_pGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)m_pGraphicsDevice->GetResolutionHeight();
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
		auto pSRV = m_pCpuTex->GetShaderResourceView();//m_pTex->GetShaderResourceView();
		pImmediateContext->PSSetShaderResources(0, 1, &pSRV);

		pImmediateContext->IASetVertexBuffers(0, arraysize(vbs), vbs, strides, offsets);
		pImmediateContext->IASetIndexBuffer(mesh->m_pIB.Get(), DXGI_FORMAT_R32_UINT, 0);

		pImmediateContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}

	// 함수이름이 자동으로 바뀌네...
	void Renderer::DrawText()
	{
		// 출력 문자 생성
		// 화면 중심이 0, 0이다.
		auto width = m_pGraphicsDevice->GetResolutionWidth();
		auto height = m_pGraphicsDevice->GetResolutionHeight();
		float drawX = (float)(((width / 2) * -1) + 0.0f);	// 계산 결과가 이상하다.
		float drawY = (float)((height / 2) - 0.0f);
		m_pFont->SetText("CHOA", DirectX::XMFLOAT2(0.0f, 200.0f));

		auto pImmediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(pImmediateContext != nullptr);

		pImmediateContext->IASetInputLayout(m_pipelineStateFont.pIL);
		pImmediateContext->IASetPrimitiveTopology(m_pipelineStateFont.primitiveTopology);
		pImmediateContext->VSSetShader(m_pipelineStateFont.pVS, NULL, 0);
		pImmediateContext->PSSetShader(m_pipelineStateFont.pPS, NULL, 0);
		pImmediateContext->PSSetSamplers(0, 1, &m_pipelineStateFont.pSS);	// 얘는 더블 포인터다...
		pImmediateContext->OMSetDepthStencilState(m_pipelineStateFont.pDSS, 1);
		pImmediateContext->RSSetState(m_pipelineStateFont.pRSS);

		pImmediateContext->VSSetConstantBuffers(0, 1, m_pCBMatrix.GetAddressOf());

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)m_pGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)m_pGraphicsDevice->GetResolutionHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		pImmediateContext->RSSetViewports(1, &viewport);

		auto pSRV = m_pFont->GetFontAtlas();
		pImmediateContext->PSSetShaderResources(0, 1, &pSRV);

		auto pVB = m_pFont->GetVertexBuffer();
		unsigned int stride = m_pFont->GetStride();
		unsigned int offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(m_pFont->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		pImmediateContext->DrawIndexed(m_pFont->GetIndexCount(), 0, 0);
	}

	void Renderer::SetGraphicsDevice(std::shared_ptr<GraphicsDevice> device)
	{
		m_pGraphicsDevice = device;

		assert(m_pGraphicsDevice);
	}	
}