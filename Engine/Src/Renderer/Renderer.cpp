#include "divepch.h"
#include "Renderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Base/Base.h"
#include "Base/Engine.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/SpriteRenderer.h"

namespace Dive
{
	// static varialbes
	GraphicsDevice Renderer::m_GraphicsDevice;

	unsigned int Renderer::m_TextureWidth = 0;
	unsigned int Renderer::m_TextureHeight = 0;

	ID3D11SamplerState* Renderer::m_pLinearSampler = nullptr;

	ID3D11InputLayout* Renderer::m_pSpriteInputLayout = nullptr;
	ID3D11VertexShader* Renderer::m_pSpriteVertexShader = nullptr;
	ID3D11PixelShader* Renderer::m_pSpritePixelShader = nullptr;

	ID3D11Buffer* Renderer::m_pMatrixBuffer = nullptr;

	// 이하 테스트
	Texture2D* Renderer::m_pSampleTex = nullptr;
	Texture2D* Renderer::m_pDepthStencilTex = nullptr;

	ID3D11DepthStencilState* Renderer::m_pDepthStencilState = nullptr;
	ID3D11RasterizerState* Renderer::m_pRasterizerState = nullptr;

	void Renderer::Initialize(const WindowData* pData)
	{
		DV_ASSERT(pData && "WnidowData가 존재하지 않습니다.");

		m_GraphicsDevice.Initialize(pData);

		// 꼭 필요한 리소스들이다.
		// 따라서 어느 하나라도 생성에 실패하면 종료하는 것이 맞다.
		// 이를 위해선 bool로 확인이 필요하며
		// 어느 부분에서 잘못되었는지 개별 추적이 가능해야 한다.
		SetTextures(pData->Width, pData->Height);
		createSamplers();
		createDepthStencilStates();
		createRasterizerStates();
		createShaders();
		createMatrixBuffer();	// 일단 임시다.
	}

	void Renderer::Shutdown()
	{
		// 함수로 만들어야 하나...?
		DV_DELETE(m_pDepthStencilTex);
		DV_DELETE(m_pSampleTex);

		m_GraphicsDevice.Shutdown();
	}
	
	// Camera를 받아 Data(viewport)를 저장
	// clear, set은 다른 곳에서?
	// Hazle은 오버로딩 되어있다.
	void Renderer::BeginScene()
	{
		auto pImmediateContext = m_GraphicsDevice.GetImmediateContext();
		auto pRenderTargetView = m_pSampleTex ? m_pSampleTex->GetRenderTargetView() : nullptr;
		auto pDepthStencilView = m_pDepthStencilTex ? m_pDepthStencilTex->GetDepthStencilView() : nullptr;
		if (!pImmediateContext || !pRenderTargetView)
			return;

		// active camera로부터 가져와야 한다.
		float clearColors[4] = {0.5f, 0.5f, 1.0f, 1.0f};

		pImmediateContext->ClearRenderTargetView(pRenderTargetView, clearColors);
		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	}
	
	void Renderer::EndScene()
	{
		auto pSwapChain = m_GraphicsDevice.GetSwapChain();
		if (!pSwapChain)
			return;

		pSwapChain->Present(m_GraphicsDevice.IsVSync() ? 1 : 0, 0);
	}

	void Renderer::DrawSprite(Transform* pTransform, SpriteRenderer* pRenderer)
	{
		if (!pTransform || !pRenderer)
			return;

		if (!pRenderer->IsEnabled())
			return;

		auto pImmediateContext = m_GraphicsDevice.GetImmediateContext();

		auto pShaderResourceView = pRenderer->GetShaderResourceView();
		pImmediateContext->PSSetShaderResources(0, 1, &pShaderResourceView);
		pImmediateContext->PSSetSamplers(0, 1, &m_pLinearSampler);
		pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
		pImmediateContext->RSSetState(m_pRasterizerState);
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// wvp constant buffer
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			
			// map
			pImmediateContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			MatrixBufferType* pPtr = static_cast<MatrixBufferType*>(mappedResource.pData);

			// world
			auto world = pTransform->GetLocalToWorld();
			auto matWorld = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&world));
			pPtr->world = matWorld;

			// view
			DirectX::XMFLOAT3 up, position, lookAt;
			DirectX::XMVECTOR upVector, positionVector, lookAtVector;
			float yaw, pitch, roll;
			DirectX::XMMATRIX rotationMatrix;

			up.x = 0.0f;
			up.y = 1.0f;
			up.z = 0.0f;
			upVector = DirectX::XMLoadFloat3(&up);

			position.x = 0.0f;
			position.y = 0.0f;
			position.z = -10.0f;
			positionVector = DirectX::XMLoadFloat3(&position);

			lookAt.x = 0.0f;
			lookAt.y = 0.0f;
			lookAt.z = 1.0f;
			lookAtVector = DirectX::XMLoadFloat3(&lookAt);

			// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
			pitch = 0.0f * 0.0174532925f;
			yaw = 0.0f * 0.0174532925f;
			roll = 0.0f * 0.0174532925f;

			// Create the rotation matrix from the yaw, pitch, and roll values.
			rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

			// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
			lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);
			upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

			// Translate the rotated camera position to the location of the viewer.
			lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

			// Finally create the view matrix from the three updated vectors.
			auto view = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
			view = DirectX::XMMatrixTranspose(view);
			pPtr->view = view;

			// proj
			// Setup the viewport for rendering.
			D3D11_VIEWPORT viewport;
			viewport.Width = (float)m_pSampleTex->GetWidth();
			viewport.Height = (float)m_pSampleTex->GetHeight();
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;

			// Create the viewport.
			pImmediateContext->RSSetViewports(1, &viewport);
			// Setup the projection matrix.
			float fieldOfView = 3.141592654f / 4.0f;
			float screenAspect = (float)m_pSampleTex->GetWidth() / (float)m_pSampleTex->GetHeight();

			// Create the projection matrix for 3D rendering.
			auto proj = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f);
			proj = DirectX::XMMatrixTranspose(proj);
			pPtr->proj = proj;

			// unmap
			pImmediateContext->Unmap(m_pMatrixBuffer, 0);

			pImmediateContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
		}

		auto pVertexBuffer = pRenderer->GetVertexBuffer();
		DV_ASSERT(pVertexBuffer);
		unsigned int stride = pRenderer->GetVertexStride();
		unsigned int offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		auto pIndexBuffer = pRenderer->GetIndexBuffer();
		DV_ASSERT(pIndexBuffer);
		auto indexCount = pRenderer->GetIndexCount();
		auto indexFormat = pRenderer->GetIndexForamt();
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, indexFormat, 0);

		pImmediateContext->IASetInputLayout(m_pSpriteInputLayout);
		pImmediateContext->VSSetShader(m_pSpriteVertexShader, nullptr, 0);
		pImmediateContext->PSSetShader(m_pSpritePixelShader, nullptr, 0);
		
		pImmediateContext->DrawIndexed(indexCount, 0, 0);
	}

	void Renderer::SetResolution(unsigned int width, unsigned int height)
	{
		m_GraphicsDevice.ResizeBackBuffer(width, height);
	}

	// 이 곳에선 크기 확인 및 저장 후 createRenderTargets()를 호출하는 편이 나을 것 같다.
	void Renderer::SetTextures(unsigned int width, unsigned int height)
	{
		if (m_TextureWidth == width && m_TextureHeight == height)
			return;

		m_TextureWidth = width;
		m_TextureHeight = height;

		createRenderTargets();
	}

	void Renderer::createRenderTargets()
	{
		unsigned int width = m_TextureWidth;
		unsigned int height = m_TextureHeight;
		
		// Render Target Textures
		DV_DELETE(m_pSampleTex);
		m_pSampleTex = Texture2D::Create(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, true);

		// Depth Stencil Buffers
		DV_DELETE(m_pDepthStencilTex);
		m_pDepthStencilTex = Texture2D::Create(width, height, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT);
	}

	void Renderer::createSamplers()
	{
		// linear
		{
			D3D11_SAMPLER_DESC desc;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			desc.BorderColor[0] = 0;
			desc.BorderColor[1] = 0;
			desc.BorderColor[2] = 0;
			desc.BorderColor[3] = 0;
			desc.MinLOD = 0;
			desc.MaxLOD = D3D11_FLOAT32_MAX;

			m_GraphicsDevice.CreateSamplerState(&desc, &m_pLinearSampler);
		}
	}

	void Renderer::createDepthStencilStates()
	{
		// 이렇게 하면 추가 괄호 안에선 초기화된 값인가...?
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		{
			// Set up the description of the stencil state.
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;

			desc.StencilEnable = true;
			desc.StencilReadMask = 0xFF;
			desc.StencilWriteMask = 0xFF;

			// Stencil operations if pixel is front-facing.
			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil operations if pixel is back-facing.
			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			m_GraphicsDevice.CreateDepthStencilState(&desc, &m_pDepthStencilState);
		}
	}
	
	void Renderer::createRasterizerStates()
	{
		D3D11_RASTERIZER_DESC desc;
		{
			desc.AntialiasedLineEnable = false;
			desc.CullMode = D3D11_CULL_BACK;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0.0f;
			desc.DepthClipEnable = true;
			desc.FillMode = D3D11_FILL_SOLID;
			desc.FrontCounterClockwise = false;
			desc.MultisampleEnable = false;
			desc.ScissorEnable = false;
			desc.SlopeScaledDepthBias = 0.0f;

			m_GraphicsDevice.CreateRasterizerState(&desc, &m_pRasterizerState);
		}
	}
	
	// 개별로 체크하는게 맞는 것 같다.
	// 따라서 이 함수가 void일 필요는 없다.
	void Renderer::createShaders()
	{
		// sprite
		{
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	28, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			// 현재 실행 경로는 Editor다.
			unsigned int numElements = ARRAYSIZE(desc);// sizeof(desc) / sizeof(desc[0]);
			m_GraphicsDevice.CreateShader("../Engine/Src/Shaders/sprite.hlsl", desc, numElements, &m_pSpriteVertexShader, &m_pSpriteInputLayout, &m_pSpritePixelShader);
		}
	}

	void Renderer::createMatrixBuffer()
	{
		{
			D3D11_BUFFER_DESC desc;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = sizeof(MatrixBufferType);
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			m_GraphicsDevice.CreateBuffer(&desc, nullptr, &m_pMatrixBuffer);
		}
	}
}