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
		DV_ASSERT(pData && "WnidowData�� �������� �ʽ��ϴ�.");

		m_GraphicsDevice.Initialize(pData);

		// �� �ʿ��� ���ҽ����̴�.
		// ���� ��� �ϳ��� ������ �����ϸ� �����ϴ� ���� �´�.
		// �̸� ���ؼ� bool�� Ȯ���� �ʿ��ϸ�
		// ��� �κп��� �߸��Ǿ����� ���� ������ �����ؾ� �Ѵ�.
		SetTextures(pData->Width, pData->Height);
		createSamplers();
		createBlendStates();
		createDepthStencilStates();
		createRasterizerStates();
		createShaders();
		createConstantBuffers();	// �ϴ� �ӽô�.

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
	
	// Camera�� �޾� Data(viewport)�� ����
	// clear, set�� �ٸ� ������?
	// Hazle�� �����ε� �Ǿ��ִ�.
	void Renderer::BeginScene()
	{
		auto pImmediateContext = m_GraphicsDevice.GetImmediateContext();
		auto pRenderTargetView = m_pGBufferAlbedo ? m_pGBufferAlbedo->GetRenderTargetView() : nullptr;
		auto pDepthStencilView = m_pDepthStencilTex ? m_pDepthStencilTex->GetDepthStencilView() : nullptr;
		if (!pImmediateContext || !pRenderTargetView)
			return;

		// active camera�κ��� �����;� �Ѵ�.
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

	// �������� ũ�Ⱑ �ܺο��� ���� �Ǿ��� ��� ȣ��
	// �׳� �Ʒ��� OnWindowData�� ������ �ű��
	// ���⿣ ���� ������ ũ����� �����ϴ� �Լ��� ����� ���� ���� �� ����.
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

	// �� ������ ũ�� Ȯ�� �� ���� �� createRenderTargets()�� ȣ���ϴ� ���� ���� �� ����.
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
			// ������ ���̾�, �ø� �׽�Ʈ ������ �Ÿ� ��
			// ����� ��� �����Ѵ�.
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

	// crateGBuffer�̶�� �ٲٴ� ���� ���� �� ����.
	// -> ���ĸ�ź�� ���� GBuffer �̿ܿ��� RenderTarget�� ����.
	void Renderer::createRenderTargets()
	{
		unsigned int width = m_TextureWidth;
		unsigned int height = m_TextureHeight;
		
		// albedo: r8g8b8a8
		DV_DELETE(m_pGBufferAlbedo);
		m_pGBufferAlbedo = Texture2D::Create(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, true);

		// normal: r8g8b8a8 or r11g11b10
		DV_DELETE(m_pGBufferNormal);
		m_pGBufferNormal = Texture2D::Create(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, true);

		// material: r8g8b8a8
		DV_DELETE(m_pGBufferMaterial);
		m_pGBufferMaterial = Texture2D::Create(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, true);

		// Depth Stencil Buffer
		DV_DELETE(m_pDepthStencilTex);
		m_pDepthStencilTex = Texture2D::Create(width, height, DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT,
			DXGI_FORMAT_R24_UNORM_X8_TYPELESS);

		// frame output
		DV_DELETE(m_pFrameOutput);
		m_pFrameOutput = Texture2D::Create(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, true);
	}

	void Renderer::removeRenderTargets()
	{
		DV_DELETE(m_pGBufferAlbedo);
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
			desc.RenderTarget[0].BlendEnable			= true;
			desc.RenderTarget[0].SrcBlend				= D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlend				= D3D11_BLEND_ONE;
			desc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha			= D3D11_BLEND_ONE;
			desc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

			m_GraphicsDevice.CreateBlendState(&desc, &m_BlendStates[static_cast<size_t>(eBlendStateType::Addtive)]);
		}
	}

	void Renderer::createDepthStencilStates()
	{
		// �̷��� �ϸ� �߰� ��ȣ �ȿ��� �ʱ�ȭ�� ���ΰ�...?
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		{
			// Set up the description of the stencil state.
			desc.DepthEnable					= true;
			desc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc						= D3D11_COMPARISON_LESS;

			desc.StencilEnable					= true;
			desc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;

			const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
			desc.FrontFace = stencilMarkOp;
			desc.BackFace = stencilMarkOp;

			// Stencil operations if pixel is front-facing.
			//desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
			//desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;
			//desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
			//desc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

			// Stencil operations if pixel is back-facing.
			//desc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
			//desc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_DECR;
			//desc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
			//desc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

			m_GraphicsDevice.CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilStateType::DepthOnStencilOn)]);
		}

		// �ӽ� forward light
		{
			D3D11_DEPTH_STENCIL_DESC descDepth;
			descDepth.DepthEnable = TRUE;
			descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			descDepth.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			descDepth.StencilEnable = FALSE;
			descDepth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			descDepth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC noSkyStencilOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
			descDepth.FrontFace = noSkyStencilOp;
			descDepth.BackFace = noSkyStencilOp;

			m_GraphicsDevice.CreateDepthStencilState(&desc, &m_DepthStencilStates[static_cast<size_t>(eDepthStencilStateType::ForwardLight)]);
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
	
	// ������ üũ�ϴ°� �´� �� ����.
	// ���� �� �Լ��� void�� �ʿ�� ����.
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
			// ���� ���� ��δ� Editor��.
			// �Լ��� ���ڸ� �ٲٸ� �� ����������.
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
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	20,	D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	32,	D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			unsigned int numElements = ARRAYSIZE(desc);
			m_GraphicsDevice.CreateShader("../Engine/Src/Shaders/Lighting.hlsl", desc, numElements,
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