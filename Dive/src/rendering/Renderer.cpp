#include "stdafx.h"
#include "Renderer.h"
#include "StaticMesh.h"
#include "Material.h"
#include "RenderPass.h"
#include "graphics/RenderTexture.h"
#include "graphics/Graphics.h"
#include "graphics/ConstantBuffer.h"
#include "graphics/Shader.h"
#include "graphics/ShaderManager.h"
#include "core/CoreDefs.h"
#include "world/World.h"
#include "world/GameObject.h"
#include "world/Components/Transform.h"
#include "world/Components/Camera.h"
#include "world/Components/MeshRenderer.h"

namespace Dive
{
	uint32_t Renderer::s_RenderTargetWidth = 0;
	uint32_t Renderer::s_RenderTargetHeight = 0;

	RenderTexture* Renderer::s_GBufferRT0 = nullptr;
	RenderTexture* Renderer::s_GBufferRT1 = nullptr;
	RenderTexture* Renderer::s_GBufferRT2 = nullptr;

	RenderTexture* Renderer::s_FrameRT = nullptr;
	RenderTexture* Renderer::s_OutputRT = nullptr;

	std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Count)> Renderer::s_RenderTargets;
	std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Count)> Renderer::s_RasterizerStates;
	std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> Renderer::s_DepthStencilStates;
	std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Count)> Renderer::s_BlendStates;

	ConstantBuffer* Renderer::s_DefaultVSConstantBuffer = nullptr;

	std::vector<std::unique_ptr<RenderPass>> Renderer::s_RenderPasses;

	void Renderer::Initialize()
	{
		ResizeRenderBuffers(Graphics::GetResolutionWidth(), Graphics::GetResolutionHeight());

		createRasterizerStates();
		createDepthStencilStates();
		createBlendStates();
		createConstantBuffers();

		s_RenderPasses.emplace_back(std::make_unique<TestPass>());

		DV_LOG(Renderer, info, "초기화 성공");
	}

	void Renderer::Shutdown()
	{	
		DV_DELETE(s_GBufferRT0);
		DV_DELETE(s_GBufferRT1);
		DV_DELETE(s_GBufferRT2);
		DV_DELETE(s_FrameRT);
		DV_DELETE(s_OutputRT);

		for (auto& rt : s_RenderTargets)
		{
			DV_DELETE(rt);
			rt = nullptr;
		}
		for (auto& rs : s_RasterizerStates)
		{
			if (rs) rs->Release();
			rs = nullptr;
		}
		for (auto& ds : s_DepthStencilStates)
		{
			if (ds) ds->Release();
			ds = nullptr;
		}
		for (auto& bs : s_BlendStates)
		{
			if (bs) bs->Release();
			bs = nullptr;
		}

		DV_LOG(Renderer, info, "셧다운 성공");
	}

	void Renderer::OnUpdate()
	{
		if (!WorldManager::GetActiveWorld())
			return;

		auto cameras = Camera::GetAllCameras();
		for (auto camera : cameras)
		{
			WorldManager::GetActiveWorld()->CullAndSort(camera);

			auto deviceContext = Graphics::GetDeviceContext();
			
			// begin frame?

			for (auto& pass : s_RenderPasses)
				pass->Execute(deviceContext, camera);

			// end frame?
		}
	}

	void Renderer::ResizeRenderBuffers(uint32_t width, uint32_t height)
	{
		if (s_RenderTargetWidth == width && s_RenderTargetHeight == height)
			return;
		
		// G-Buffer
		{
			if (!s_GBufferRT0)
			{
				s_GBufferRT0 = new RenderTexture(width, height);
				s_GBufferRT0->Create();
			}
			else
			{
				s_GBufferRT0->Resize(width, height);
			}

			if (!s_GBufferRT1)
			{
				s_GBufferRT1 = new RenderTexture(width, height);
				s_GBufferRT1->Create();
			}
			else
			{
				s_GBufferRT1->Resize(width, height);
			}

			if (!s_GBufferRT2)
			{
				s_GBufferRT2 = new RenderTexture(width, height);
				s_GBufferRT2->Create();
			}
			else
			{
				s_GBufferRT2->Resize(width, height);
			}
		}

		// FrameRender
		// FrameOutput
		
		if (!s_RenderTargets[static_cast<size_t>(eRenderTarget::FrameOutput)])
		{
			s_RenderTargets[static_cast<size_t>(eRenderTarget::FrameOutput)] = new RenderTexture(width, height);
			s_RenderTargets[static_cast<size_t>(eRenderTarget::FrameOutput)]->Create();
		}
		else
		{
			s_RenderTargets[static_cast<size_t>(eRenderTarget::FrameOutput)]->Resize(width, height);
		}

		s_RenderTargetWidth = width;
		s_RenderTargetHeight = height;
	}
	
	RenderTexture* Renderer::GetGBuffer(eGBuffer type)
	{
		switch (type)
		{
		case eGBuffer::Rt0:
			return s_GBufferRT0;
		case eGBuffer::Rt1:
			return s_GBufferRT1;
		case eGBuffer::Rt2:
			return s_GBufferRT2;

		default:
			return nullptr;
		}
	}

	RenderTexture* Renderer::GetRenderTarget(eRenderTarget type)
	{
		return type != eRenderTarget::Count ? s_RenderTargets[static_cast<size_t>(type)] : nullptr;
	}

	ID3D11RasterizerState* Renderer::GetRasterizerState(eRasterizerState type)
	{
		return type != eRasterizerState::Count ? s_RasterizerStates[static_cast<size_t>(type)] : nullptr;
	}

	ID3D11DepthStencilState* Renderer::GetDepthStencilState(eDepthStencilState type)
	{
		return type != eDepthStencilState::Count ? s_DepthStencilStates[static_cast<size_t>(type)] : nullptr;
	}

	ID3D11BlendState* Renderer::GetBlendState(eBlendState type) 
	{ 
		return type != eBlendState::Count ? s_BlendStates[static_cast<size_t>(type)] : nullptr; 
	}
	
	void Renderer::createConstantBuffers()
	{
		s_DefaultVSConstantBuffer = new ConstantBuffer();
		s_DefaultVSConstantBuffer->Create(sizeof(DefaultVSConstant));
	}

	void Renderer::createShaders()
	{
		// vertex shaders

		// pixel shaders
	}

	void Renderer::createRasterizerStates()
	{
		D3D11_RASTERIZER_DESC desc{};

		// FillSolid_CullBack
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
			DV_LOG(Renderer, err, "RasterizerState FillSolid_CullBack 생성 실패");
		}

		// FillSolid_CullNone
		desc.CullMode = D3D11_CULL_NONE;
		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &s_RasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullNone)])))
		{
			DV_LOG(Renderer, err, "RasterizerState FillSolid_CullNode 생성 실패");
		}
	}
	
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
				DV_LOG(Renderer, err, "DepthStencilState DepthReadWrite 생성 실패");
			}
		}

		// DepthReadWrite_StencilReadWrite => Skydome에서 on
		// => rastertek에서 가장 최초에 사용하는 것
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
				DV_LOG(Renderer, err, "DepthStencilState DepthReadWrite_StencilReadWrite 생성 실패");
			}
		}

		// GBuffer에도 하나 있다.
		// DepthReadWrite_StencilReadWrite
		// 하지만 Face 설정이 위와 다르다.
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
				DV_LOG(Renderer, err, "DepthStencilState GBuffer 생성 실패");
			}
		}

		// skydome에서 depth off용
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
				DV_LOG(Renderer, err, "DepthDisabledStencilState 생성 실패");
			}
		}

		// forwardLight
		{
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			desc.StencilEnable = FALSE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC noSkyStencilOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
			desc.FrontFace = noSkyStencilOp;
			desc.BackFace = noSkyStencilOp;

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_DepthStencilStates[static_cast<size_t>(eDepthStencilState::ForwardLight)])))
			{
				DV_LOG(Renderer, err, "DepthDisabledStencilState 생성 실패");
			}
		}
	}
	
	void Renderer::createBlendStates()
	{
		// Addictive
		D3D11_BLEND_DESC desc{};
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
			DV_LOG(Renderer, err, "BlandState Addictive 생성 실패");
		}
	}
}
