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

#include "resource/ResourceManager.h"
#include "graphics/Texture2D.h"
#include "graphics/Cubemap.h"

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
	std::array<ID3D11DepthStencilState*, static_cast<size_t>(eDepthStencilState::Count)> Renderer::s_depthStencilStates;
	std::array<Microsoft::WRL::ComPtr<ID3D11BlendState>, static_cast<size_t>(eBlendState::Count)> Renderer::s_blendStates;
	std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, static_cast<size_t>(eSamplerState::Count)> Renderer::s_samplerStates;

	std::unique_ptr<LightManager> Renderer::s_lightManager;

	bool Renderer::Initialize()
	{
		ResizeRenderBuffers(Graphics::GetWidth(), Graphics::GetHeight());

		CreateRasterizerStates();
		CreateDepthStencilStates();
		CreateBlendStates();
		CreateSamplerStates();

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
		for (auto& ds : s_depthStencilStates)
		{
			if (ds) ds->Release();
			ds = nullptr;
		}

		DV_LOG(Renderer, info, "셧다운 성공");
	}

	void Renderer::OnRender()
	{
		if (!WorldManager::GetActiveWorld())
			return;

		Renderer::BindSamplerStates();

		const auto world = WorldManager::GetActiveWorld();
		
		auto cameras = Camera::GetAllCameras();
		for (auto cameraCom : cameras)
		{
			// 임시: 에디터 모드가 아니라면 이걸 실행시키는 게 맞다.
			// 좀 더 생각해보니 Editor와 Main 둘 중 하나만 그리는 게 맞다.
			// 물론 그외의 것들이 존재할 수 있다.
			//if (cameraCom->GetGameObject()->GetTag() == "EditorOnly")
			//	continue;

			// editor mode일때
			if (cameraCom->GetGameObject()->GetTag() == "MainCamera")
				continue;

			WorldManager::GetActiveWorld()->CullAndSort(cameraCom);
			// 위치가 애매하다. 하지만 바로 위에서 업데이트된 후에 호출하는 게 맞다.
			s_lightManager->Update(WorldManager::GetActiveWorld());
			
			// forward rendering path
			// Camera에서 RenderingPath를 구분
			{
				cameraCom->Bind();	// RenerTarget Clear 포함
				s_lightManager->Bind();
				
				// sky box
				{
					Graphics::GetDeviceContext()->OMSetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::Skybox), 0);
					Graphics::GetDeviceContext()->RSSetState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullFront));
					
					auto cubemap = ResourceManager::GetByName<Cubemap>("cloudy_skybox");
					cubemap->Bind(eShaderResourceSlot::SkyMap);

					ShaderManager::GetProgram("Skybox")->Bind();

					Graphics::GetDeviceContext()->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
					Graphics::GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					Graphics::GetDeviceContext()->Draw(36, 0);
				}

				// Shadow pass
				{

				}

				// ForwardBase pass
				// 하나의 Directional Light만 적용
				{
					ForwardBase pass;
					pass.Execute(cameraCom);
				}

				// ForwardAdd pass
				// Point, Spot Light를 적용
				{

				}

				
				// Transparent pass
				{
					Transparent pass;
					pass.Execute(cameraCom);
				}

				// ResolveScene
				{
				//	ResolveScene pass;
				//	pass.Execute(cameraCom);
				}
			}
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
		return type != eDepthStencilState::Count ? s_depthStencilStates[static_cast<size_t>(type)] : nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D11BlendState> Renderer::GetBlendState(eBlendState type)
	{
		return type != eBlendState::Count ? s_blendStates[static_cast<size_t>(type)] : nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D11SamplerState> Renderer::GetSamplerState(eSamplerState type)
	{
		return type != eSamplerState::Count ? s_samplerStates[static_cast<size_t>(type)] : nullptr;
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

		// FillSolid_CullFront
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_FRONT;
		desc.FrontCounterClockwise = TRUE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = TRUE;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.AntialiasedLineEnable = FALSE;

		if (FAILED(Graphics::GetDevice()->CreateRasterizerState(&desc, &s_rasterizerStates[static_cast<size_t>(eRasterizerState::FillSolid_CullFront)])))
		{
			DV_LOG(Renderer, err, "RasterizerState FillSolid_CullFront 생성 실패");
		}

		// FillSolid_CullBack
		desc.FrontCounterClockwise = FALSE;
		desc.CullMode = D3D11_CULL_BACK;
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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthStencilStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite)])))
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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthStencilStates[static_cast<size_t>(eDepthStencilState::DepthReadWrite_StencilReadWrite)])))
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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthStencilStates[static_cast<size_t>(eDepthStencilState::GBuffer)])))
			{
				DV_LOG(Renderer, err, "DepthStencilState GBuffer 생성 실패");
			}
		}

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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthStencilStates[static_cast<size_t>(eDepthStencilState::DepthDiabled)])))
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

			if (FAILED(Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthStencilStates[static_cast<size_t>(eDepthStencilState::ForwardLight)])))
			{
				DV_LOG(Renderer, err, "DepthDisabledStencilState 생성 실패");
			}
		}

		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_LESS;

			auto hr = Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthStencilStates[static_cast<size_t>(eDepthStencilState::Transparent)]);
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateDepthStencilStates] Transparent 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}
		}

		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = FALSE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			desc.StencilEnable = FALSE;

			auto hr = Graphics::GetDevice()->CreateDepthStencilState(&desc, &s_depthStencilStates[static_cast<size_t>(eDepthStencilState::Skybox)]);
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateDepthStencilStates] Skybox 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}

		}
	}

	void Renderer::CreateBlendStates()
	{
		// Alpha
		{
			D3D11_BLEND_DESC desc{};
			desc.AlphaToCoverageEnable = FALSE;
			desc.IndependentBlendEnable = FALSE;

			const D3D11_RENDER_TARGET_BLEND_DESC alphaBlendDesc =
			{
				TRUE,                                // BlendEnable
				D3D11_BLEND_SRC_ALPHA,               // SrcBlend
				D3D11_BLEND_INV_SRC_ALPHA,           // DestBlend
				D3D11_BLEND_OP_ADD,                  // BlendOp

				D3D11_BLEND_ONE,                     // SrcBlendAlpha
				D3D11_BLEND_INV_SRC_ALPHA,           // DestBlendAlpha
				D3D11_BLEND_OP_ADD,                  // BlendOpAlpha

				D3D11_COLOR_WRITE_ENABLE_ALL         // RenderTargetWriteMask
			};

			for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
				desc.RenderTarget[i] = alphaBlendDesc;
			
			HRESULT hr = Graphics::GetDevice()->CreateBlendState(&desc, &s_blendStates[static_cast<size_t>(eBlendState::AlphaEnabled)]);
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateBlendState] AlphaEnabled 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}

			for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
				desc.RenderTarget[i].BlendEnable = FALSE;

			hr = Graphics::GetDevice()->CreateBlendState(&desc, &s_blendStates[static_cast<size_t>(eBlendState::AlphaDisabled)]);
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateBlendState] AlpahDisabled 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}
		}

		// Addictive
		{
			D3D11_BLEND_DESC desc{};
			desc.AlphaToCoverageEnable = FALSE;
			desc.IndependentBlendEnable = FALSE;
			const D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
			{
				TRUE,
				D3D11_BLEND_ONE, 
				D3D11_BLEND_ONE, 
				D3D11_BLEND_OP_ADD,
				
				D3D11_BLEND_ONE, 
				D3D11_BLEND_ONE, 
				D3D11_BLEND_OP_ADD,
				
				D3D11_COLOR_WRITE_ENABLE_ALL,
			};
			for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
				desc.RenderTarget[i] = defaultRenderTargetBlendDesc;

			auto hr = Graphics::GetDevice()->CreateBlendState(&desc, &s_blendStates[static_cast<size_t>(eBlendState::Addictive)]);
			if(FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateBlendState] Addictive 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}
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
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.BorderColor[0] = 0;
			samplerDesc.BorderColor[1] = 0;
			samplerDesc.BorderColor[2] = 0;
			samplerDesc.BorderColor[3] = 0;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			auto hr = device->CreateSamplerState(&samplerDesc, s_samplerStates[static_cast<size_t>(eSamplerState::WrapLinear)].GetAddressOf());
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateSamplerStates] WrapLinear Sampler 생성 실패: {}", ErrorUtils::ToVerbose(hr));
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
				DV_LOG(Renderer, err, "[::CreateSamplerStates] ClampPoint Sampler 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}
		}

		// ClampLinear
		{
			D3D11_SAMPLER_DESC samplerDesc{};
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;


			auto hr = device->CreateSamplerState(&samplerDesc, s_samplerStates[static_cast<size_t>(eSamplerState::ClampLinear)].GetAddressOf());
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateSamplerStates] ClampLinear Sampler 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}
		}

		// Skybox
		{
			D3D11_SAMPLER_DESC samplerDesc{};
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP; 
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			auto hr = device->CreateSamplerState(&samplerDesc, s_samplerStates[static_cast<size_t>(eSamplerState::Skybox)].GetAddressOf());
			if (FAILED(hr))
			{
				DV_LOG(Renderer, err, "[::CreateSamplerStates] Skybox Sampler 생성 실패: {}", ErrorUtils::ToVerbose(hr));
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
				DV_LOG(Renderer, err, "[::CreateSamplerStates] ShadowCompare Sampler 생성 실패: {}", ErrorUtils::ToVerbose(hr));
				return;
			}
		}
	}
}
