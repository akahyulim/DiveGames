#include "DivePch.h"
#include "Renderer.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/ShaderVariation.h"
#include "Graphics/InputLayout.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/RenderTexture.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/Light.h"
#include "Scene/Components/MeshRenderer.h"
#include "Scene/Components/SkinnedMeshRenderer.h"

namespace Dive
{
	static DirectX::XMFLOAT2 s_ResolutionRender;

	static std::vector<Layer*> s_Layers;

	static std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Count)> s_RasterizerStates;
	static std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> s_DepthStencilStates;
	static std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Count)> s_BlendStates;

	static std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Count)> s_RenderTargets;
	
	// ��� ���̴��� ���ҽ� �Ŵ����� �����ϴ� ���� ���� �� ����.
	static ShaderVariation* s_pDeferredShadingVertexShader = nullptr;
	static ShaderVariation* s_pDeferredShadingPixelShader = nullptr;
	static ShaderVariation* s_pDeferredSkinnedShadingVertexShader = nullptr;
	static ShaderVariation* s_pDeferredSkinnedShadingPixelShader = nullptr;
	static ShaderVariation* s_pDeferredDirLightVertexShader = nullptr;
	static ShaderVariation* s_pDeferredDirLightPixelShader = nullptr;
	static ShaderVariation* s_pSkydomeVertexShader = nullptr;
	static ShaderVariation* s_pSkydomePixelShader = nullptr;

	// ����Ű�� �������ڸ�
	// ���̴� ��ü�� ���� ��������� ũ�� �� ���Ǹ� �����Ͽ�
	// �����о��� ��Ƽ���󿡼� ���������� �� 
	// ���̴��� ���ε��ϴ� ������� ������ �� �ִ�.
	static ConstantBuffer* s_pCameraVertexShaderBuffer = nullptr;
	static ConstantBuffer* s_pModelVertexShaderBuffer = nullptr;
	static ConstantBuffer* s_pCameraPixelShaderBuffer = nullptr;
	static ConstantBuffer* s_pLightPixelShaderBuffer = nullptr;
	static ConstantBuffer* s_pMaterialPixelShaderBuffer = nullptr;

	bool Renderer::Initialize()
	{
		// ���� �� �κж����� createRenderTargets�� �� �� ȣ��ȴ�.
		// �̰� ���ĸ�ź�� ���� �����ϴ�.
		SetResolutionRender(Graphics::GetWindowSize());

		if (!createShaders())
		{
			DV_CORE_WARN("���̴� ��ü ������ �����Ͽ����ϴ�.");
		}

		if (!createConstantBuffers())
		{
			DV_CORE_WARN("CreateBuffers ������ �����Ͽ����ϴ�.");
		}

		createRasterizerStates();
		createDepthStencilStates();
		createBlendStates();
		
		createRenderTargets();
		
		DV_CORE_INFO("Renderer �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void Renderer::Shutdown()
	{
		for (auto* pViewport : s_Layers)
		{
			DV_DELETE(pViewport);
		}

		{
			DV_DELETE(s_pMaterialPixelShaderBuffer);
			DV_DELETE(s_pLightPixelShaderBuffer);
			DV_DELETE(s_pCameraPixelShaderBuffer);
			DV_DELETE(s_pModelVertexShaderBuffer);
			DV_DELETE(s_pCameraVertexShaderBuffer);
		}

		for (auto* pRasterizerState : s_RasterizerStates)
			DV_RELEASE(pRasterizerState);

		for (auto* pDepthStencilState : s_DepthStencilStates)
			DV_RELEASE(pDepthStencilState);

		for (auto* pBlendState : s_BlendStates)
			DV_RELEASE(pBlendState);

		for (auto* pRenderTarget : s_RenderTargets)
			DV_DELETE(pRenderTarget);
		

		DV_CORE_INFO("Renderer ���ῡ �����Ͽ����ϴ�.");
	}

	void Renderer::Update()
	{
		for (auto* pViewport : s_Layers)
			pViewport->Update();
	}

	void Renderer::Render()
	{
		for (auto* pViewport : s_Layers)
			pViewport->Render();
	}

	DirectX::XMFLOAT2 Renderer::GetResolutionRender()
	{
		return s_ResolutionRender;
	}
	
	void Renderer::SetResolutionRender(uint32_t width, uint32_t height)
	{
		if (s_ResolutionRender.x == static_cast<float>(width) || s_ResolutionRender.y == static_cast<float>(height))
			return;

		s_ResolutionRender.x = static_cast<float>(width);
		s_ResolutionRender.y = static_cast<float>(height);

		createRenderTargets();
		// ���ĸ�ź�� ���÷��� ���� �����.
		// �� ��� ���÷��� �ؽ�ó�� �ִ���...?
	}

	void Renderer::SetResolutionRender(DirectX::XMINT2 size)
	{
		SetResolutionRender(size.x, size.y);
	}

	RenderTexture* Renderer::GetRenderTarget(const eRenderTarget renderTarget)
	{
		return s_RenderTargets[static_cast<size_t>(renderTarget)];
	}

	ID3D11DepthStencilState* Renderer::GetDepthStencilState(const eDepthStencilState state)
	{
		return s_DepthStencilStates[static_cast<size_t>(state)];
	}

	ID3D11RasterizerState* Renderer::GetRasterizerState(const eRasterizerState state)
	{
		return s_RasterizerStates[static_cast<size_t>(state)];
	}

	ID3D11BlendState* Renderer::GetBlendState(const eBlendState state)
	{
		return s_BlendStates[static_cast<size_t>(state)];
	}

	void Renderer::PushLayer(Layer* pViewport)
	{
		s_Layers.emplace_back(pViewport);
	}

	void Renderer::SetLayer(uint32_t index, Layer* pViewport)
	{
		if (index >= GetNumLayers())
			s_Layers.resize(index + 1);

		s_Layers[index] = pViewport;
	}

	uint32_t Renderer::GetNumLayers()
	{
		return static_cast<uint32_t>(s_Layers.size());
	}

	ShaderVariation* Renderer::GetDeferredShadingVertexShaderVariation()
	{
		return s_pDeferredShadingVertexShader;
	}

	ShaderVariation* Renderer::GetDeferredShadingPixelShaderVariation()
	{
		return s_pDeferredShadingPixelShader;
	}

	ShaderVariation* Renderer::GetDeferredSkinnedShadingVertexShaderVariation()
	{
		return s_pDeferredSkinnedShadingVertexShader;
	}

	ShaderVariation* Renderer::GetDeferredSkinnedShadingPixelShaderVariation()
	{
		return s_pDeferredSkinnedShadingPixelShader;
	}

	ShaderVariation* Renderer::GetDeferredDirLightVertexShaderVariation()
	{
		return s_pDeferredDirLightVertexShader;
	}

	ShaderVariation* Renderer::GetDeferredDirLightPixelShaderVariation()
	{
		return s_pDeferredDirLightPixelShader;
	}

	ShaderVariation* Renderer::GetSkydomeShaderVariation(eShaderType type)
	{
		return type == eShaderType::VertexShader ? s_pSkydomeVertexShader : s_pSkydomePixelShader;
	}

	ConstantBuffer* Renderer::GetCameraVertexShaderBuffer()
	{
		return s_pCameraVertexShaderBuffer;
	}

	ConstantBuffer* Renderer::GetModelVertexShaderBuffer()
	{
		return s_pModelVertexShaderBuffer;
	}

	ConstantBuffer* Renderer::GetCameraPixelShaderBuffer()
	{
		return s_pCameraPixelShaderBuffer;
	}

	ConstantBuffer* Renderer::GetLightPixelShaderBuffer()
	{
		return s_pLightPixelShaderBuffer;
	}

	ConstantBuffer* Renderer::GetMaterialPixelShaderBuffer()
	{
		return s_pMaterialPixelShaderBuffer;
	}

	void Renderer::createRasterizerStates()
	{
		D3D11_RASTERIZER_DESC desc;

		// FillSolid_CullBack
		ZeroMemory(&desc, sizeof(desc));
		desc.AntialiasedLineEnable = FALSE;
		desc.CullMode = D3D11_CULL_BACK;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.ScissorEnable = FALSE;
		desc.SlopeScaledDepthBias = 0.0f;

		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &s_RasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullBack)])))
		{
			DV_CORE_ERROR("RasterizerState FillSolid_CullBack ������ �����Ͽ����ϴ�.");
		}

		// FillSolid_CullNone
		desc.CullMode = D3D11_CULL_NONE;
		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &s_RasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullNone)])))
		{
			DV_CORE_ERROR("RasterizerState FillSolid_CullNode ������ �����Ͽ����ϴ�.");
		}
	}

	// DepthWriteMask���� ZERO�� ����, ALL�� �ѱ�. READ�� �⺻?
	void Renderer::createDepthStencilStates()
	{
		D3D11_DEPTH_STENCIL_DESC desc;

		// DepthReadWrite
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = FALSE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
			desc.FrontFace = stencilMarkOp;
			desc.BackFace = stencilMarkOp;
		
			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite)])))
			{
				DV_CORE_ERROR("DepthStencilState DepthReadWrite ������ �����Ͽ����ϴ�.");
			}
		}

		// DepthReadWrite_StencilReadWrite => Skydome���� on
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite_StencilReadWrite)])))
			{
				DV_CORE_ERROR("DepthStencilState DepthReadWrite_StencilReadWrite ������ �����Ͽ����ϴ�.");
			}
		}

		// GBuffer���� �ϳ� �ִ�.
		// DepthReadWrite_StencilReadWrite
		// ������ Face ������ ���� �ٸ���.
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
			desc.FrontFace = stencilMarkOp;
			desc.BackFace = stencilMarkOp;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_DepthStencilStates[static_cast<size_t>(eDepthStencilState::GBuffer)])))
			{
				DV_CORE_ERROR("DepthStencilState GBuffer ������ �����Ͽ����ϴ�.");
			}
		}

		// skydome���� depth off��
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = FALSE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = 0xFF;
			desc.StencilWriteMask = 0xFF;
			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_DepthStencilStates[static_cast<size_t>(eDepthStencilState::DepthDiabled)])))
			{
				DV_CORE_ERROR("DepthDisabledStencilState ������ �����Ͽ����ϴ�.");
			}
		}
	}

	void Renderer::createBlendStates()
	{
		// Addictive
		D3D11_BLEND_DESC desc;
		desc.AlphaToCoverageEnable = FALSE;
		desc.IndependentBlendEnable = FALSE;
		const D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
		{
			TRUE,
			D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
			D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
			D3D11_COLOR_WRITE_ENABLE_ALL,
		};
		for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			desc.RenderTarget[i] = defaultRenderTargetBlendDesc;

		if (FAILED(Graphics::GetDevice()->CreateBlendState(&desc, &s_BlendStates[static_cast<size_t>(eBlendState::Addictive)])))
		{
			DV_CORE_ERROR("BlandState Addictive ������ �����Ͽ����ϴ�.");
		}
	}

	// ����Ƽ�� ��� material���� �����ϴ� shader�� legacy�� standard �� �����θ� ������.
	// Light �� post procssing�� ��� �����ϴ��� ���� �𸣰ڴ�.
	bool Renderer::createShaders()
	{
		// Deferred Shading
		{
			s_pDeferredShadingVertexShader = new ShaderVariation;
			if (!s_pDeferredShadingVertexShader->CompileAndCreate(eShaderType::VertexShader, "CoreData/Shaders/Deferred.hlsl", eVertexType::Model))
				return false;
			s_pDeferredShadingPixelShader = new ShaderVariation;
			if (!s_pDeferredShadingPixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/Deferred.hlsl"))
				return false;

			s_pDeferredSkinnedShadingVertexShader = new ShaderVariation;
			if (!s_pDeferredSkinnedShadingVertexShader->CompileAndCreate(eShaderType::VertexShader, "CoreData/Shaders/DeferredSkinned.hlsl", eVertexType::Skinned))
				return false;
			s_pDeferredSkinnedShadingPixelShader = new ShaderVariation;
			if (!s_pDeferredSkinnedShadingPixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/DeferredSkinned.hlsl"))
				return false;

			s_pDeferredDirLightVertexShader = new ShaderVariation;
			if (!s_pDeferredDirLightVertexShader->CompileAndCreate(eShaderType::VertexShader, "CoreData/Shaders/DeferredDirLight.hlsl"))
				return false;
			s_pDeferredDirLightPixelShader = new ShaderVariation;
			if (!s_pDeferredDirLightPixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/DeferredDirLight.hlsl"))
				return false;
		}

		// skydome
		{
			s_pSkydomeVertexShader = new ShaderVariation;
			if (!s_pSkydomeVertexShader->CompileAndCreate(eShaderType::VertexShader, "CoreData/Shaders/Skydome.hlsl", eVertexType::Model))
				return false;

			s_pSkydomePixelShader = new ShaderVariation;
			if (!s_pSkydomePixelShader->CompileAndCreate(eShaderType::PixelShader, "CoreData/Shaders/Skydome.hlsl"))
				return false;
		}

		return true;
	}

	bool Renderer::createConstantBuffers()
	{
		// camera vertex shader
		{
			s_pCameraVertexShaderBuffer = new ConstantBuffer("CameraVertexShaderBuffer");
			if (!s_pCameraVertexShaderBuffer->Create<CameraVertexShaderBuffer>())
				return false;
		}

		// model vertex shader
		{
			s_pModelVertexShaderBuffer = new ConstantBuffer("ModelVertexShaderBuffer");
			if (!s_pModelVertexShaderBuffer->Create<ModelVertexShaderBuffer>())
				return false;
		}

		// camera pixel shader
		{
			s_pCameraPixelShaderBuffer = new ConstantBuffer("CameraPixelShaderBuffer");
			if (!s_pCameraPixelShaderBuffer->Create<CameraPixelShaderBuffer>())
				return false;
		}

		// light pixel shader
		{
			s_pLightPixelShaderBuffer = new ConstantBuffer("LightPixelShaderBuffer");
			if (!s_pLightPixelShaderBuffer->Create<LightPixelShaderBuffer>())
				return false;
		}

		// material pixel shader
		{
			s_pMaterialPixelShaderBuffer = new ConstantBuffer("MaterialPixelShaderBuffer");
			if (!s_pMaterialPixelShaderBuffer->Create<MaterialPixelShaderBuffer>())
				return false;
		}

		return true;
	}

	// ���� ��� ����Ÿ���� ����� ũ��� �����ȴ�.
	void Renderer::createRenderTargets()
	{
		for (auto* pRenderTarget : s_RenderTargets)
			DV_DELETE(pRenderTarget);

		int width = static_cast<int>(s_ResolutionRender.x);
		int height = static_cast<int>(s_ResolutionRender.y);

		// GBuffer_DepthStencil
		auto* pGbufferDepthStencil = new RenderTexture;
		pGbufferDepthStencil->SetDepthStencil(width, height, DXGI_FORMAT_R24G8_TYPELESS, true);
		pGbufferDepthStencil->SetFilter(D3D11_FILTER_MIN_MAG_MIP_POINT);
		pGbufferDepthStencil->UpdateSamplerState();
		s_RenderTargets[static_cast<size_t>(eRenderTarget::GBuffer_DepthStencil)] = pGbufferDepthStencil;
			
		// GBuffer_Color_SpecIntensity
		auto* pGBufferColorSpecIntensity = new RenderTexture;
		pGBufferColorSpecIntensity->SetRenderTarget(width, height);
		pGBufferColorSpecIntensity->UpdateSamplerState();
		s_RenderTargets[static_cast<size_t>(eRenderTarget::GBuffer_Color_SpecIntensity)] = pGBufferColorSpecIntensity;
			
		// GBuffer_Normal
		auto* pGBufferNormal = new RenderTexture;
		pGBufferNormal->SetRenderTarget(width, height, DXGI_FORMAT_R11G11B10_FLOAT);
		pGBufferNormal->UpdateSamplerState();
		s_RenderTargets[static_cast<size_t>(eRenderTarget::GBuffer_Normal)] = pGBufferNormal;

		// GBuffer_SpecPower
		auto* pGBufferSpecPower = new RenderTexture;
		pGBufferSpecPower->SetRenderTarget(width, height);
		pGBufferSpecPower->UpdateSamplerState();
		s_RenderTargets[static_cast<size_t>(eRenderTarget::GBuffer_SpecPower)] = pGBufferSpecPower;

		// FrameRender
		auto* pFrameRender = new RenderTexture;
		pFrameRender->SetRenderTarget(width, height);
		s_RenderTargets[static_cast<size_t>(eRenderTarget::FrameRender)] = pFrameRender;
			
		// FrameOutput

		DV_CORE_INFO("RenderTargets({0:d} x {1:d})�� ���� �Ǵ� �����Ͽ����ϴ�.", width, height);
	}
}