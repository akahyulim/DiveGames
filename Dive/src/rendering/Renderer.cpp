#include "stdafx.h"
#include "Renderer.h"
#include "StaticMesh.h"
#include "Material.h"
#include "core/EventDispatcher.h"
#include "graphics/RenderTexture.h"
#include "graphics/Graphics.h"
#include "graphics/Shader.h"
#include "graphics/ShaderManager.h"
#include "world/World.h"
#include "world/WorldManager.h"
#include "world/GameObject.h"
#include "world/Components/Transform.h"
#include "world/Components/Camera.h"
#include "world/Components/MeshRenderer.h"

namespace Dive
{
	uint64_t Renderer::s_frameCount = 0;

	uint32_t Renderer::s_renderTargetWidth = 0;
	uint32_t Renderer::s_renderTargetHeight = 0;

	RenderTexture* Renderer::s_gBufferRT0 = nullptr;
	RenderTexture* Renderer::s_gBufferRT1 = nullptr;
	RenderTexture* Renderer::s_gBufferRT2 = nullptr;

	RenderTexture* Renderer::s_frameRenderTarget = nullptr;
	RenderTexture* Renderer::s_outputRenderTarget = nullptr;

	std::array<RenderTexture*, static_cast<size_t>(eRenderTarget::Count)> Renderer::s_renderTargets;
	std::array<ID3D11RasterizerState*, static_cast<size_t>(eRasterizerState::Count)> Renderer::s_rasterizerStates;
	std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> Renderer::s_depthSteniclStates;
	std::array<ID3D11BlendState*, static_cast<size_t>(eBlendState::Count)> Renderer::s_blendStates;
	std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, static_cast<size_t>(eSamplerState::Count)> Renderer::s_samplerStates;

	std::vector<std::unique_ptr<RenderPass>> Renderer::s_renderPasses;
	std::unique_ptr<LightManager> Renderer::s_lightManager;

	bool Renderer::Initialize()
	{
		ResizeRenderBuffers(Graphics::GetWidth(), Graphics::GetHeight());

		CreateRasterizerStates();
		CreateDepthStencilStates();
		CreateBlendStates();
		CreateSamplerStates();

		s_renderPasses.emplace_back(std::make_unique<ForwardLighting>());

		s_lightManager = std::make_unique<LightManager>();

		DV_SUBSCRIBE_EVENT(eEventType::Render, DV_EVENT_HANDLER_STATIC(OnRender));

		DV_LOG(Renderer, info, "초기화 성공");

		return true;
	}

	void Renderer::Shutdown()
	{
		DV_DELETE(s_gBufferRT0);
		DV_DELETE(s_gBufferRT1);
		DV_DELETE(s_gBufferRT2);
		DV_DELETE(s_frameRenderTarget);
		DV_DELETE(s_outputRenderTarget);

		for (auto& rt : s_renderTargets)
		{
			DV_DELETE(rt);
			rt = nullptr;
		}
		for (auto& rs : s_rasterizerStates)
		{
			if (rs) rs->Release();
			rs = nullptr;
		}
		for (auto& ds : s_depthSteniclStates)
		{
			if (ds) ds->Release();
			ds = nullptr;
		}
		for (auto& bs : s_blendStates)
		{
			if (bs) bs->Release();
			bs = nullptr;
		}

		DV_LOG(Renderer, info, "셧다운 성공");
	}

	void Renderer::OnRender()
	{
		Render();
	}

	// Render를 분할하고 싶다.
	// 분류도 이 곳에서 하고 싶다.
	void Renderer::Render()
	{
		if (!WorldManager::GetActiveWorld())
			return;

		auto cameras = Camera::GetAllCameras();
		for (auto camera : cameras)
		{
			WorldManager::GetActiveWorld()->CullAndSort(camera);
			// 위치가 애매하다. 하지만 바로 위에서 업데이트된 후에 호출하는 게 맞다.
			s_lightManager->Update(WorldManager::GetActiveWorld());

			// begin frame?

			// pass의 묶음은 path이다.
			// 추후 Forward, Deferred로 나뉘어야 한다.
			for (auto& pass : s_renderPasses)
			{
				// 역시 위치가 애매하다.
				s_lightManager->Bind();
				pass->Execute(camera);
			}

			// end frame?
		}

		s_frameCount++;
	}

	void Renderer::ResizeRenderBuffers(uint32_t width, uint32_t height)
	{
		if (s_renderTargetWidth == width && s_renderTargetHeight == height)
			return;

		// G-Buffer
		{
			if (!s_gBufferRT0)
			{
				s_gBufferRT0 = new RenderTexture(width, height);
				s_gBufferRT0->Create();
			}
			else
			{
				s_gBufferRT0->Resize(width, height);
			}

			if (!s_gBufferRT1)
			{
				s_gBufferRT1 = new RenderTexture(width, height);
				s_gBufferRT1->Create();
			}
			else
			{
				s_gBufferRT1->Resize(width, height);
			}

			if (!s_gBufferRT2)
			{
				s_gBufferRT2 = new RenderTexture(width, height);
				s_gBufferRT2->Create();
			}
			else
			{
				s_gBufferRT2->Resize(width, height);
			}
		}

		// FrameRender
		// FrameOutput

		if (!s_renderTargets[static_cast<size_t>(eRenderTarget::FrameOutput)])
		{
			s_renderTargets[static_cast<size_t>(eRenderTarget::FrameOutput)] = new RenderTexture(width, height);
			s_renderTargets[static_cast<size_t>(eRenderTarget::FrameOutput)]->Create();
		}
		else
		{
			s_renderTargets[static_cast<size_t>(eRenderTarget::FrameOutput)]->Resize(width, height);
		}

		s_renderTargetWidth = width;
		s_renderTargetHeight = height;
	}

	RenderTexture* Renderer::GetGBuffer(eGBuffer type)
	{
		switch (type)
		{
		case eGBuffer::Rt0:
			return s_gBufferRT0;
		case eGBuffer::Rt1:
			return s_gBufferRT1;
		case eGBuffer::Rt2:
			return s_gBufferRT2;

		default:
			return nullptr;
		}
	}

	RenderTexture* Renderer::GetRenderTarget(eRenderTarget type)
	{
		return type != eRenderTarget::Count ? s_renderTargets[static_cast<size_t>(type)] : nullptr;
	}

	ID3D11RasterizerState* Renderer::GetRasterizerState(eRasterizerState type)
	{
		return type != eRasterizerState::Count ? s_rasterizerStates[static_cast<size_t>(type)] : nullptr;
	}

	ID3D11DepthStencilState* Renderer::GetDepthStencilState(eDepthStencilState type)
	{
		return type != eDepthStencilState::Count ? s_depthSteniclStates[static_cast<size_t>(type)] : nullptr;
	}

	ID3D11BlendState* Renderer::GetBlendState(eBlendState type)
	{
		return type != eBlendState::Count ? s_blendStates[static_cast<size_t>(type)] : nullptr;
	}

	ID3D11SamplerState* Renderer::GetSamplerState(eSamplerState type)
	{
		return type != eSamplerState::Count ? s_samplerStates[static_cast<size_t>(type)].Get() : nullptr;
	}

	void Renderer::BindSamplerStates()
	{
		auto deviceContext = Graphics::GetDeviceContext();
		std::array<ID3D11SamplerState*, static_cast<size_t>(eSamplerState::Count)> samplerStates{};
		for (size_t i = 0; i < samplerStates.size(); ++i)
			samplerStates[i] = s_samplerStates[i].Get();

		deviceContext->PSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
	}

	void Renderer::CreateRasterizerStates()
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

		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &s_rasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullBack)])))
		{
			DV_LOG(Renderer, err, "RasterizerState FillSolid_CullBack 생성 실패");
		}

		// FillSolid_CullNone
		desc.CullMode = D3D11_CULL_NONE;
		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &s_rasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullNone)])))
		{
			DV_LOG(Renderer, err, "RasterizerState FillSolid_CullNode 생성 실패");
		}
	}

	void Renderer::CreateDepthStencilStates()
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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthSteniclStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite)])))
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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthSteniclStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite_StencilReadWrite)])))
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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthSteniclStates[static_cast<size_t>(eDepthStencilState::GBuffer)])))
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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthSteniclStates[static_cast<size_t>(eDepthStencilState::DepthDiabled)])))
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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthSteniclStates[static_cast<size_t>(eDepthStencilState::ForwardLight)])))
			{
				DV_LOG(Renderer, err, "DepthDisabledStencilState 생성 실패");
			}
		}
	}

	void Renderer::CreateBlendStates()
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

		if (FAILED(Graphics::GetDevice()->CreateBlendState(&desc, &s_blendStates[static_cast<size_t>(eBlendState::Addictive)])))
		{
			DV_LOG(Renderer, err, "BlandState Addictive 생성 실패");
		}
	}

	void Renderer::CreateSamplerStates()
	{
		auto device = Graphics::GetDevice();

		// WrapLinear
		{
			D3D11_SAMPLER_DESC samplerDesc{};
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			auto hr = device->CreateSamplerState(&samplerDesc, s_samplerStates[static_cast<size_t>(eSamplerState::WrapLinear)].GetAddressOf());
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateSamplerStates] WrapLinear SamplerState 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}
		}

		// ClampPoint
		{
			D3D11_SAMPLER_DESC samplerDesc{};
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

			auto hr = device->CreateSamplerState(&samplerDesc, s_samplerStates[static_cast<size_t>(eSamplerState::ClampPoint)].GetAddressOf());
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateSamplerStates] ClampPoint SamplerState 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}
		}

		// ClampCubeLinear
		{
			D3D11_SAMPLER_DESC samplerDesc{};
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

			auto hr = device->CreateSamplerState(&samplerDesc, s_samplerStates[static_cast<size_t>(eSamplerState::ClampCubeLinear)].GetAddressOf());
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateSamplerStates] ClampCubeLinear SamplerState 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}
		}

		// ShadowCompare
		{
			D3D11_SAMPLER_DESC samplerDesc{};
			samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
			samplerDesc.BorderColor[0] = 1.0f;
			samplerDesc.BorderColor[1] = 1.0f;
			samplerDesc.BorderColor[2] = 1.0f;
			samplerDesc.BorderColor[3] = 1.0f;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0.0f;

			auto hr = device->CreateSamplerState(&samplerDesc, s_samplerStates[static_cast<size_t>(eSamplerState::ShadowCompare)].GetAddressOf());
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateSamplerStates] ShadowCompare SamplerState 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}
		}
	}
}
