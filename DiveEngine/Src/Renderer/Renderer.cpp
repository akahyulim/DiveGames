#include "DivePch.h"
#include "Renderer.h"
#include "View.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/ShaderVariation.h"
#include "Graphics/InputLayout.h"
#include "Graphics/ConstantBuffer.h"
#include "IO/Log.h"

namespace Dive
{
	static std::vector<View*> s_Views;

	// Shader는 Cache에서 관리하는 것이 맞을 듯 하다.
	// 그리고 Drawable, Light 등이 각자 포인터를 가지는 거다.
	// 문제는 어느 시점에 누가 생성을 하느냐 이다.
	// urho는 Renderer와 Graphics 둘 다 관여했던 것 같다.
	static ShaderVariation* s_pForwardLightVertexShader = nullptr;
	static ShaderVariation* s_pDirectionalLightPixelShader = nullptr;
	static ShaderVariation* s_pPointLightPixelShader = nullptr;
	static ShaderVariation* s_pDeferredShadingVertexShader = nullptr;
	static ShaderVariation* s_pDeferredShadingPixelShader = nullptr;
	static ShaderVariation* s_pDeferredDirLightVertexShader = nullptr;
	static ShaderVariation* s_pDeferredDirLightPixelShader = nullptr;

	static ConstantBuffer* s_pCameraVertexShaderBuffer = nullptr;
	static ConstantBuffer* s_pModelVertexShaderBuffer = nullptr;
	static ConstantBuffer* s_pCameraPixelShaderBuffer = nullptr;
	static ConstantBuffer* s_pLightPixelShaderBuffer = nullptr;
	static ConstantBuffer* s_pMaterialPixelShaderBuffer = nullptr;

	static ID3D11DepthStencilState* s_pDepthStencilState = nullptr;
	static ID3D11DepthStencilState* s_pForwardLightDS = nullptr;

	static ID3D11RasterizerState* s_pRasterizerState = nullptr;

	static ID3D11BlendState* s_pBlendState = nullptr;

	bool Renderer::Initialize()
	{
		if (!createShaders())
		{
			DV_CORE_WARN("셰이더 객체 생성에 실패하였습니다.");
		}

		if (!createConstantBuffers())
		{
			DV_CORE_WARN("CreateBuffers 생성에 실패하였습니다.");
		}

		if (!createDepthStencilStates())
		{
			DV_CORE_WARN("DepthStencilStates 생성에 실패하였습니다.");
		}

		if (!createRasterizerStates())
		{
			DV_CORE_WARN("RasterizerStates 생성에 실패하였습니다.");
		}

		if (!createBlendStates())
		{
			DV_CORE_WARN("BlendStates 생성에 실패하였습니다.");
		}
		
		DV_CORE_TRACE("Renderer 초기화에 성공하였습니다.");

		return true;
	}

	void Renderer::Shutdown()
	{
		{
			DV_DELETE(s_pMaterialPixelShaderBuffer);
			DV_DELETE(s_pLightPixelShaderBuffer);
			DV_DELETE(s_pCameraPixelShaderBuffer);
			DV_DELETE(s_pModelVertexShaderBuffer);
			DV_DELETE(s_pCameraVertexShaderBuffer);

			DV_RELEASE(s_pRasterizerState);
			DV_RELEASE(s_pDepthStencilState);
			DV_DELETE(s_pDirectionalLightPixelShader);
			DV_DELETE(s_pPointLightPixelShader);
			DV_DELETE(s_pForwardLightVertexShader);
		}

		for (auto pView : s_Views)
			DV_DELETE(pView);

		DV_CORE_TRACE("Renderer 종료에 성공하였습니다.");
	}

	void Renderer::Update(float delta)
	{
		for (auto pView : s_Views)
			pView->Update(delta);
	}

	void Renderer::Render()
	{
		for (auto pView : s_Views)
			pView->Render();
	}

	View* Renderer::GetView(uint32_t index)
	{
		return GetViewCount() > index ? s_Views[index] : nullptr;
	}

	void Renderer::SetView(uint32_t index, View* pView)
	{
		if (GetViewCount() >= index)
			s_Views.insert(s_Views.begin() + index, pView);
		else
			s_Views.emplace_back(pView);
	}

	uint32_t Renderer::GetViewCount()
	{
		return static_cast<uint32_t>(s_Views.size());
	}

	ShaderVariation* Renderer::GetForwardLightVertexShaderVariation()
	{
		return s_pForwardLightVertexShader;
	}

	ShaderVariation* Renderer::GetDirectionalLightPixelShaderVariation()
	{
		return s_pDirectionalLightPixelShader;
	}

	ShaderVariation* Renderer::GetPointLightPixelShaderVariation()
	{
		return s_pPointLightPixelShader;
	}

	ShaderVariation* Renderer::GetDeferredShadingVertexShaderVariation()
	{
		return s_pDeferredShadingVertexShader;
	}

	ShaderVariation* Renderer::GetDeferredShadingPixelShaderVariation()
	{
		return s_pDeferredShadingPixelShader;
	}

	ShaderVariation* Renderer::GetDeferredDirLightVertexShaderVariation()
	{
		return s_pDeferredDirLightVertexShader;
	}

	ShaderVariation* Renderer::GetDeferredDirLightPixelShaderVariation()
	{
		return s_pDeferredDirLightPixelShader;
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

	ID3D11DepthStencilState* Renderer::GetDepthStencilState()
	{
		return s_pDepthStencilState;
	}

	ID3D11DepthStencilState* Renderer::GetForwardLightDS()
	{
		return s_pForwardLightDS;
	}

	ID3D11RasterizerState* Renderer::GetRasterizerState()
	{
		return s_pRasterizerState;
	}

	ID3D11BlendState* Renderer::GetBlendState()
	{
		return s_pBlendState;
	}

	bool Renderer::createShaders()
	{
		// forward light
		{
			s_pForwardLightVertexShader = new ShaderVariation;
			if (!s_pForwardLightVertexShader->CompileAndCreate(eShaderType::VertexShader, "../CoreData/Shaders/ForwardLightCommon.hlsl", eVertexType::Model))
				return false;
			s_pDirectionalLightPixelShader = new ShaderVariation;
			if (!s_pDirectionalLightPixelShader->CompileAndCreate(eShaderType::PixelShader, "../CoreData/Shaders/DirectionalLight.hlsl"))
				return false;
			s_pPointLightPixelShader = new ShaderVariation;
			if (!s_pPointLightPixelShader->CompileAndCreate(eShaderType::PixelShader, "../CoreData/Shaders/PointLight.hlsl"))
				return false;
		}

		// Deferred Shading
		{
			s_pDeferredShadingVertexShader = new ShaderVariation;
			if (!s_pDeferredShadingVertexShader->CompileAndCreate(eShaderType::VertexShader, "../CoreData/Shaders/Deferred.hlsl", eVertexType::Model))
				return false;
			s_pDeferredShadingPixelShader = new ShaderVariation;
			if (!s_pDeferredShadingPixelShader->CompileAndCreate(eShaderType::PixelShader, "../CoreData/Shaders/Deferred.hlsl"))
				return false;

			s_pDeferredDirLightVertexShader = new ShaderVariation;
			if (!s_pDeferredDirLightVertexShader->CompileAndCreate(eShaderType::VertexShader, "../CoreData/Shaders/DeferredDirLight.hlsl"))
				return false;
			s_pDeferredDirLightPixelShader = new ShaderVariation;
			if (!s_pDeferredDirLightPixelShader->CompileAndCreate(eShaderType::PixelShader, "../CoreData/Shaders/DeferredDirLight.hlsl"))
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

	bool Renderer::createDepthStencilStates()
	{
		DV_ASSERT(Graphics::IsInitialized());

		D3D11_DEPTH_STENCIL_DESC desc;

		// default?
		{
			ZeroMemory(&desc, sizeof(desc));

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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_pDepthStencilState)))
			{
				DV_CORE_ERROR("DepthStencilState 생성에 실패하였습니다.");
				return false;
			}
		}

		// forwardLightDS
		{
			ZeroMemory(&desc, sizeof(desc));

			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			desc.StencilEnable = FALSE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC noSkyStencilOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
			desc.FrontFace = noSkyStencilOp;
			desc.BackFace = noSkyStencilOp;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_pForwardLightDS)))
			{
				DV_CORE_ERROR("DepthStencilState 생성에 실패하였습니다.");
				return false;
			}
		}

		return true;
	}

	bool Renderer::createRasterizerStates()
	{
		DV_ASSERT(Graphics::IsInitialized());

		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
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

		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &s_pRasterizerState)))
		{
			DV_CORE_ERROR("RasterizerState 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool Renderer::createBlendStates()
	{
		// addictive blend state
		{
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
			
			if (FAILED(Graphics::GetDevice()->CreateBlendState(&desc, &s_pBlendState)))
			{
				DV_CORE_ERROR("BlandState 생성에 실패하였습니다.");
				return false;
			}
		}

		return true;
	}
}