#include "stdafx.h"
#include "RenderingPipeline.h"
#include "graphics/RenderTexture.h"
#include "graphics/Graphics.h"
#include "core/CoreDefs.h"

namespace Dive
{
	UINT32 RenderingPipeline::s_RenderTargetWidth = 0;
	UINT32 RenderingPipeline::s_RenderTargetHeight = 0;

	RenderTexture* RenderingPipeline::s_GBufferRT0 = nullptr;
	RenderTexture* RenderingPipeline::s_GBufferRT1 = nullptr;
	RenderTexture* RenderingPipeline::s_GBufferRT2 = nullptr;

	RenderTexture* RenderingPipeline::s_FrameRT = nullptr;
	RenderTexture* RenderingPipeline::s_OutputRT = nullptr;

	std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Count)> RenderingPipeline::s_RenderTargets;
	std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Count)> RenderingPipeline::s_RasterizerStates;
	std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> RenderingPipeline::s_DepthStencilStates;
	std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Count)> RenderingPipeline::s_BlendStates;

	void RenderingPipeline::Initialize()
	{
		ResizeRenderBuffers(Graphics::GetResolutionWidth(), Graphics::GetResolutionHeight());

		createRasterizerStates();
		createDepthStencilStates();
		createBlendStates();
	}

	void RenderingPipeline::Shutdown()
	{	
		
	}

	// 카메라, 렌더링 대상 메시들
	void RenderingPipeline::DeferredPass()
	{
		// clear gbuffer
		// draw on gbuffer
	}

	void RenderingPipeline::LightingPass()
	{
		// clear rendertarget
		// draw light
	}

	void RenderingPipeline::ResizeRenderBuffers(UINT32 width, UINT32 height)
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

		if (!s_RenderTargets[static_cast<size_t>(eRenderTarget::Editor_WorldView)])
		{
			s_RenderTargets[static_cast<size_t>(eRenderTarget::Editor_WorldView)] = new RenderTexture(width, height);
			s_RenderTargets[static_cast<size_t>(eRenderTarget::Editor_WorldView)]->Create();
		}
		else
		{
			s_RenderTargets[static_cast<size_t>(eRenderTarget::Editor_WorldView)]->Resize(width, height);
		}

		if (!s_RenderTargets[static_cast<size_t>(eRenderTarget::Editor_GameView)])
		{
			s_RenderTargets[static_cast<size_t>(eRenderTarget::Editor_GameView)] = new RenderTexture(width, height);
			s_RenderTargets[static_cast<size_t>(eRenderTarget::Editor_GameView)]->Create();
		}
		else
		{
			s_RenderTargets[static_cast<size_t>(eRenderTarget::Editor_GameView)]->Resize(width, height);
		}

		s_RenderTargetWidth = width;
		s_RenderTargetHeight = height;
	}
	
	RenderTexture* RenderingPipeline::GetGBuffer(eGBuffer type)
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

	RenderTexture* RenderingPipeline::GetRenderTarget(eRenderTarget type)
	{
		return type != eRenderTarget::Count ? s_RenderTargets[static_cast<size_t>(type)] : nullptr;
	}

	ID3D11RasterizerState* RenderingPipeline::GetRasterizerState(eRasterizerState type)
	{
		return type != eRasterizerState::Count ? s_RasterizerStates[static_cast<size_t>(type)] : nullptr;
	}

	ID3D11DepthStencilState* RenderingPipeline::GetDepthStencilState(eDepthStencilState type)
	{
		return type != eDepthStencilState::Count ? s_DepthStencilStates[static_cast<size_t>(type)] : nullptr;
	}

	ID3D11BlendState* RenderingPipeline::GetBlendState(eBlendState type) 
	{ 
		return type != eBlendState::Count ? s_BlendStates[static_cast<size_t>(type)] : nullptr; 
	}
	
	void RenderingPipeline::createShaders()
	{
		// vertex shaders

		// pixel shaders
	}

	void RenderingPipeline::createRasterizerStates()
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

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
			DV_LOG(RenderingPipeline, err, "RasterizerState FillSolid_CullBack 생성 실패");
		}

		// FillSolid_CullNone
		desc.CullMode = D3D11_CULL_NONE;
		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &s_RasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullNone)])))
		{
			DV_LOG(RenderingPipeline, err, "RasterizerState FillSolid_CullNode 생성 실패");
		}
	}
	
	void RenderingPipeline::createDepthStencilStates()
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
				DV_LOG(RenderingPipeline, err, "DepthStencilState DepthReadWrite 생성에 실패하였습니다.");
			}
		}

		// DepthReadWrite_StencilReadWrite => Skydome에서 on
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
				DV_LOG(RenderingPipeline, err, "DepthStencilState DepthReadWrite_StencilReadWrite 생성에 실패하였습니다.");
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
				DV_LOG(RenderingPipeline, err, "DepthStencilState GBuffer 생성에 실패하였습니다.");
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
				DV_LOG(RenderingPipeline, err, "DepthDisabledStencilState 생성에 실패하였습니다.");
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
				DV_LOG(RenderingPipeline, err, "DepthDisabledStencilState 생성에 실패하였습니다.");
			}
		}
	}
	
	void RenderingPipeline::createBlendStates()
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
			DV_LOG(RenderingPipeline, err, "BlandState Addictive 생성에 실패하였습니다.");
		}
	}
}