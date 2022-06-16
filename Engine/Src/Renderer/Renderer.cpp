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
#include "Scene/Component/Light.h"
#include "Graphics/TextureCube.h"	// test

namespace Dive
{
	// static varialbes
	GraphicsDevice Renderer::m_GraphicsDevice;

	unsigned int Renderer::m_TextureWidth = 0;
	unsigned int Renderer::m_TextureHeight = 0;

	std::array<ID3D11SamplerState*, static_cast<size_t>(eSamplerStateType::Count)> Renderer::m_SamplerStates;
	std::array<ID3D11BlendState*, static_cast<size_t>(eBlendStateType::Count)> Renderer::m_BlendStates;
	std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilStateType::Count)> Renderer::m_DepthStencilStates;
	std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerStateType::Count)> Renderer::m_RasterizerStates;
	std::array<Shader, static_cast<size_t>(eShaderType::Count)> Renderer::m_Shaders;

	ConstantBuffer* Renderer::m_pCbFrame = nullptr;
	ConstantBuffer* Renderer::m_pCbUber = nullptr;
	ConstantBuffer* Renderer::m_pCbSprite = nullptr;
	ConstantBuffer* Renderer::m_pCbLight = nullptr;

	D3D11_VIEWPORT Renderer::m_Viewport;

	Texture2D* Renderer::m_pGBufferAlbedo = nullptr;
	Texture2D* Renderer::m_pGBufferNormal = nullptr;
	Texture2D* Renderer::m_pGBufferMaterial = nullptr;
	Texture2D* Renderer::m_pDepthStencilTex = nullptr;

	Texture2D* Renderer::m_pFrameOutput = nullptr;

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
		createBlendStates();
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
		DV_DELETE(m_pGBufferAlbedo);

		m_GraphicsDevice.Shutdown();
	}
	
	// Camera를 받아 Data(viewport)를 저장
	// clear, set은 다른 곳에서?
	// Hazle은 오버로딩 되어있다.
	void Renderer::BeginScene()
	{
		auto pImmediateContext = m_GraphicsDevice.GetImmediateContext();
		auto pRenderTargetView = m_pGBufferAlbedo ? m_pGBufferAlbedo->GetRenderTargetView() : nullptr;
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

	ID3D11BlendState* Renderer::GetBlendState(eBlendStateType type)
	{
		if (type >= eBlendStateType::Count)
			return nullptr;

		return m_BlendStates[static_cast<size_t>(type)];
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

			// mesh renderalbes
			{
				auto pMeshRenderable = pGameObject->GetComponent<MeshRenderable>();
				if (pMeshRenderable && pMeshRenderable->IsEnabled())
					vis.visibleMeshRenderables.emplace_back(pGameObject);
			}

			// lights
			{
				auto pLight = pGameObject->GetComponent<Light>();
				if (pLight && pLight->IsEnabled())
					vis.visibleLights.emplace_back(pGameObject);
			}
		}
	}

	// crateGBuffer이라고 바꾸는 편이 나을 것 같다.
	// -> 스파르탄을 보니 GBuffer 이외에도 RenderTarget이 많다.
	// 리소스의 생성과 업데이트를 분리하고 싶다.
	// 현재 Texture2D의 경우 SetSize를 통해 재생성이 가능하기 때문이다.
	void Renderer::createRenderTargets()
	{
		unsigned int width = m_TextureWidth;
		unsigned int height = m_TextureHeight;
		
		// albedo: r8g8b8a8
		if (!m_pGBufferAlbedo)
		{
			m_pGBufferAlbedo = new Texture2D;
		}
		m_pGBufferAlbedo->SetSize(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, eTextureUsage::TEXTURE_RENDERTARGET);

		// normal: r8g8b8a8 or r11g11b10
		if (!m_pGBufferNormal)
		{
			m_pGBufferNormal = new Texture2D;
		}
		m_pGBufferNormal->SetSize(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, eTextureUsage::TEXTURE_RENDERTARGET);

		// material: r8g8b8a8
		if (!m_pGBufferMaterial)
		{
			m_pGBufferMaterial = new Texture2D;
		}
		m_pGBufferMaterial->SetSize(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, eTextureUsage::TEXTURE_RENDERTARGET);

		// Depth Stencil Buffer
		if (!m_pDepthStencilTex)
		{
			m_pDepthStencilTex = new Texture2D;
		}
		m_pDepthStencilTex->SetSize(width, height, DXGI_FORMAT_R24G8_TYPELESS, eTextureUsage::TEXTURE_DEPTHSTENCIL);

		// frame output
		if (!m_pFrameOutput)
		{
			m_pFrameOutput = new Texture2D;
		}
		m_pFrameOutput->SetSize(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, eTextureUsage::TEXTURE_RENDERTARGET);
	}

	void Renderer::removeRenderTargets()
	{
		DV_DELETE(m_pGBufferAlbedo);
		DV_DELETE(m_pDepthStencilTex);
	}

	void Renderer::createSamplers()
	{
		// point
		{
			D3D11_SAMPLER_DESC desc;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
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

			m_GraphicsDevice.CreateSamplerState(&desc, &m_SamplerStates[static_cast<size_t>(eSamplerStateType::Point)]);
		}

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

	void Renderer::createBlendStates()
	{
		D3D11_BLEND_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		// disabled
		{
			desc.AlphaToCoverageEnable					= false;
			desc.IndependentBlendEnable					= false;
			desc.RenderTarget[0].BlendEnable			= false;
			desc.RenderTarget[0].SrcBlend				= D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_ONE;
			desc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

			m_GraphicsDevice.CreateBlendState(&desc, &m_BlendStates[static_cast<size_t>(eBlendStateType::Disabled)]);
		}

		// alpha
		{
			desc.AlphaToCoverageEnable					= false;
			desc.IndependentBlendEnable					= false;
			desc.RenderTarget[0].BlendEnable			= true;
			desc.RenderTarget[0].SrcBlend				= D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_ONE;
			desc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

			m_GraphicsDevice.CreateBlendState(&desc, &m_BlendStates[static_cast<size_t>(eBlendStateType::Alpha)]);
		}

		// addtive
		{
			desc.AlphaToCoverageEnable					= false;
			desc.IndependentBlendEnable					= false;
			//desc.RenderTarget[0].BlendEnable			= true;
			//desc.RenderTarget[0].SrcBlend				= D3D11_BLEND_ONE;
			//desc.RenderTarget[0].DestBlend				= D3D11_BLEND_ONE;
			//desc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
			//desc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
			//desc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_ONE;
			//desc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
			//desc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
			const D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
			{
				TRUE,
				D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
				D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
				D3D11_COLOR_WRITE_ENABLE_ALL,
			};
			desc.RenderTarget[0] = defaultRenderTargetBlendDesc;
			//for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			//	desc.RenderTarget[i] = defaultRenderTargetBlendDesc;

			m_GraphicsDevice.CreateBlendState(&desc, &m_BlendStates[static_cast<size_t>(eBlendStateType::Addtive)]);
		}
	}

	// 일단 책에 있는 것들로만 우선 만들었다.
	// 추가될 여지가 있다.
	void Renderer::createDepthStencilStates()
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		// dpeth off, stencil off
		{
			// depth
			desc.DepthEnable					= false;
			desc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc						= D3D11_COMPARISON_NEVER;

			// stencil
			desc.StencilEnable					= false;
			desc.StencilWriteMask				= 0;
			desc.StencilReadMask				= 0;
			// face
			desc.FrontFace.StencilFunc			= D3D11_COMPARISON_NEVER;
			desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
			desc.BackFace						= desc.FrontFace;

			m_GraphicsDevice.CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilStateType::DepthOff_StencilOff)]);
		}

		// less / write, ds
		{
			// depth
			desc.DepthEnable					= true;
			desc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc						= D3D11_COMPARISON_LESS;

			// stencil
			desc.StencilEnable					= true;
			desc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;
			desc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
			// face
			desc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
			desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_REPLACE;
			desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_REPLACE;
			desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_REPLACE;
			desc.BackFace						= desc.FrontFace;

			m_GraphicsDevice.CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilStateType::DepthWriteLess_StencilOn)]);
		}

		// greater / no write, ds
		{
			// depth
			desc.DepthEnable					= true;
			desc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc						= D3D11_COMPARISON_GREATER_EQUAL;

			// stencil
			desc.StencilEnable					= false;
			desc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;
			desc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
			// face
			desc.FrontFace.StencilFunc			= D3D11_COMPARISON_EQUAL;
			desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP; 
			desc.BackFace						= desc.FrontFace;

			m_GraphicsDevice.CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilStateType::DepthNoWriteGreater_StencilOn)]);
		}

		// less / no write, ds
		{
			// depth
			desc.DepthEnable					= true;
			desc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc						= D3D11_COMPARISON_LESS;

			// stencil
			desc.StencilEnable = true;
			desc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;
			desc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
			// face
			desc.FrontFace.StencilFunc			= D3D11_COMPARISON_EQUAL;
			desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP; 
			desc.BackFace						= desc.FrontFace;

			m_GraphicsDevice.CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilStateType::DepthNoWriteLess_StencilOn)]);
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
				{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	12, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

		// Deferred
		{
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	20,	D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	32,	D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			unsigned int numElements = ARRAYSIZE(desc);
			m_GraphicsDevice.CreateShader("../Engine/Src/Shaders/DeferredShading.hlsl", desc, numElements,
				&m_Shaders[static_cast<size_t>(eShaderType::Deferred)].pVertexShader,
				&m_Shaders[static_cast<size_t>(eShaderType::Deferred)].pInputLayout,
				&m_Shaders[static_cast<size_t>(eShaderType::Deferred)].pPixelShader);
		}

		// light
		{
			m_GraphicsDevice.CreateShader("../Engine/Src/Shaders/Lighting.hlsl", nullptr, 0,
				&m_Shaders[static_cast<size_t>(eShaderType::Light)].pVertexShader,
				&m_Shaders[static_cast<size_t>(eShaderType::Light)].pInputLayout,
				&m_Shaders[static_cast<size_t>(eShaderType::Light)].pPixelShader);
		}
	}

	void Renderer::createConstantBuffers()
	{
		m_pCbFrame = new ConstantBuffer;
		m_pCbFrame->Create<FrameBuffer>();

		m_pCbUber = new ConstantBuffer;
		m_pCbUber->Create<UberBuffer>();

		m_pCbSprite = new ConstantBuffer;
		m_pCbSprite->Create<SpriteBuffer>();

		m_pCbLight = new ConstantBuffer;
		m_pCbLight->Create<LightBuffer>();
	}
}