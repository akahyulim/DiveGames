#include "divepch.h"
#include "Renderer.h"
#include "ConstantBuffers.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/ConstantBuffer.h"
#include "Base/Engine.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/SpriteRenderable.h"

namespace Dive
{
	// static varialbes
	GraphicsDevice Renderer::m_GraphicsDevice;

	unsigned int Renderer::m_TextureWidth = 0;
	unsigned int Renderer::m_TextureHeight = 0;

	std::array<ID3D11SamplerState*, static_cast<size_t>(eSamplerStateType::Count)> Renderer::m_SamplerStates;
	std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilStateType::Count)> Renderer::m_DepthStencilStates;
	std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerStateType::Count)> Renderer::m_RasterizerStates;
	std::array<Shader, static_cast<size_t>(eShaderType::Count)> Renderer::m_Shaders;

	ConstantBuffer* Renderer::m_pCbFrame = nullptr;
	ConstantBuffer* Renderer::m_pCbUber = nullptr;

	D3D11_VIEWPORT Renderer::m_Viewport;

	// 이하 테스트
	Texture2D* Renderer::m_pSampleTex = nullptr;
	Texture2D* Renderer::m_pDepthStencilTex = nullptr;

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
		createConstantBuffers();	// 일단 임시다.

		SUBSCRIBE_EVENT(eEventType::WindowData, EVENT_HANDLER_STATIC(OnWindowData));
	}

	void Renderer::Shutdown()
	{
		UNSUBSCRIBE_EVENT(eEventType::WindowData, EVENT_HANDLER_STATIC(OnWindowData));


		for (auto pSamplerState : m_SamplerStates)
		{
			DV_RELEASE(pSamplerState);
		}

		for (auto pDepthStencilState : m_DepthStencilStates)
		{
			DV_RELEASE(pDepthStencilState);
		}

		for (auto pRasterizerState : m_RasterizerStates)
		{
			DV_RELEASE(pRasterizerState);
		}
	
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

	// 윈도우의 크기가 외부에서 변경 되었을 경우 호출
	// 그냥 아래의 OnWindowData로 내용을 옮기고
	// 여기엔 직접 윈도우 크기까지 변경하는 함수로 만드는 것이 나을 것 같다.
	void Renderer::SetResolution(unsigned int width, unsigned int height)
	{
		m_GraphicsDevice.ResizeBackBuffer(width, height);

		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.Width	= static_cast<FLOAT>(width);
		m_Viewport.Height	= static_cast<FLOAT>(height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
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

	void Renderer::OnWindowData(const Event& e)
	{
		const WindowDataEvent& windowDataEvent = dynamic_cast<const WindowDataEvent&>(e);
		const auto& data = windowDataEvent.GetWindowData();

		if (data.msg == WM_SIZE && data.wParam != SIZE_MINIMIZED)
		{
			SetResolution(data.Width, data.Height);
		}
	}

	ID3D11SamplerState* Renderer::GetSamplerState(eSamplerStateType type)
	{
		if(type >= eSamplerStateType::Count)
			return nullptr;

		return m_SamplerStates[static_cast<size_t>(type)];
	}

	ID3D11DepthStencilState* Renderer::GetDepthStencilState(eDepthStencilStateType type)
	{
		if(type >= eDepthStencilStateType::Count)
			return nullptr;

		return m_DepthStencilStates[static_cast<size_t>(type)];
	}

	ID3D11RasterizerState* Renderer::GetRasterizerState(eRasterizerStateType type)
	{
		if(type >= eRasterizerStateType::Count)
			return nullptr;

		return m_RasterizerStates[static_cast<size_t>(type)];
	}

	Shader* Renderer::GetShader(eShaderType type)
	{
		if (type >= eShaderType::Count)
			return nullptr;

		return &m_Shaders[static_cast<size_t>(type)];
	}

	void Renderer::UpdateVisibility(Visibility& vis)
	{
		vis.Clear();

		auto pScene = vis.pScene;
		for(auto pGameObject : pScene->GetGameObjects())
		{
			// 원래는 레이어, 컬링 테스트 등으로 거른 후
			// 통과한 대상만 저장한다.
			//if (vis.flags & Visibility::SpriteRenderables)
			{
				auto pSpriteRenderable = pGameObject->GetComponent<SpriteRenderable>();
				if (pSpriteRenderable && pSpriteRenderable->IsEnabled())
					vis.visibleSpriteRenderables.emplace_back(pGameObject);
			}

			{
				auto pMeshRenderable = pGameObject->GetComponent<MeshRenderable>();
				if (pMeshRenderable && pMeshRenderable->IsEnabled())
					vis.visibleMeshRenderables.emplace_back(pGameObject);
			}
		}
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

	void Renderer::removeRenderTargets()
	{
		DV_DELETE(m_pSampleTex);
		DV_DELETE(m_pDepthStencilTex);
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

			m_GraphicsDevice.CreateSamplerState(&desc, &m_SamplerStates[static_cast<size_t>(eSamplerStateType::Linear)]);
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

			m_GraphicsDevice.CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilStateType::DepthOnStencilOn)]);
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
			m_GraphicsDevice.CreateRasterizerState(&desc, &m_RasterizerStates[static_cast<size_t>(eRasterizerStateType::CullBackSolid)]);
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

			unsigned int numElements = ARRAYSIZE(desc);// sizeof(desc) / sizeof(desc[0]);
			// 현재 실행 경로는 Editor다.
			// 함수의 인자를 바꾸면 더 간단해진다.
			m_GraphicsDevice.CreateShader("../Engine/Src/Shaders/sprite.hlsl", desc, numElements,
				&m_Shaders[static_cast<size_t>(eShaderType::Sprite)].pVertexShader,
				&m_Shaders[static_cast<size_t>(eShaderType::Sprite)].pInputLayout,
				&m_Shaders[static_cast<size_t>(eShaderType::Sprite)].pPixelShader);
		}

		// mesh
		{
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	20,	D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	32,	D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			unsigned int numElements = ARRAYSIZE(desc);
			m_GraphicsDevice.CreateShader("../Engine/Src/Shaders/Mesh.hlsl", desc, numElements,
				&m_Shaders[static_cast<size_t>(eShaderType::Mesh)].pVertexShader,
				&m_Shaders[static_cast<size_t>(eShaderType::Mesh)].pInputLayout,
				&m_Shaders[static_cast<size_t>(eShaderType::Mesh)].pPixelShader);
		}
	}

	void Renderer::createConstantBuffers()
	{
		m_pCbFrame = new ConstantBuffer;
		m_pCbFrame->Create<FrameBuffer>();

		m_pCbUber = new ConstantBuffer;
		m_pCbUber->Create<UberBuffer>();
	}
}