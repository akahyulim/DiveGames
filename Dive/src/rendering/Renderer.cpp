#include "stdafx.h"
#include "Renderer.h"
#include "core/CoreDefs.h"
#include "graphics/Graphics.h"
#include "graphics/Texture2D.h"

namespace Dive
{
	Renderer::Renderer()
		: m_Width(0)
		, m_Height(0)
		, m_FrameCount(0)
	{
	}

	Renderer::~Renderer()
	{
		Shutdown();
	}

	bool Renderer::Initialize(uint32_t width, uint32_t height, std::shared_ptr<Graphics> pGraphics)
	{
		m_pGraphics = pGraphics;

		ResizeTargets(width, height);

		DV_LOG(Renderer, trace, "초기화 성공");

		return true;
	}

	void Renderer::Shutdown()
	{
		DV_LOG(Renderer, trace, "셧다운 성공(frame count: {:d})", m_FrameCount);
	}

	void Renderer::Tick()
	{
		// update
		{
		}

		// render
		{
			// viewport
			D3D11_VIEWPORT viewport{ 0.0f, 0.0f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.0f, 1.0f };
			m_pGraphics->GetDeviceContext()->RSSetViewports(1, &viewport);

			// render targets
			ID3D11RenderTargetView* renderTargets[] = {
				GetRenderTargetView(eRenderTargetType::GBuffer_Diffuse),
				GetRenderTargetView(eRenderTargetType::GBuffer_Normal),
				GetRenderTargetView(eRenderTargetType::GBuffer_Specular),
				GetRenderTargetView(eRenderTargetType::FrameRender)
			};

			// 현재 GetDepthStencilView를 만들기엔 조금 애매하다.
			// 텍스쳐와 enum class도 별도로 분리해야 할 것 같다.
			ID3D11DepthStencilView* pDepthStencilView = m_RenderTargets[static_cast<size_t>(eRenderTargetType::GBuffer_DepthStencil)]->GetDepthStencilView();

			float clearColor[4] = { 1.0f, 0.1f, 0.1f, 1.0f };
			for (int i = 0; i != 4; i++)
			{
				m_pGraphics->GetDeviceContext()->ClearRenderTargetView(renderTargets[i], clearColor);
			}
			m_pGraphics->GetDeviceContext()->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			m_pGraphics->GetDeviceContext()->OMSetRenderTargets(3, renderTargets, pDepthStencilView);

			DV_FIRE_EVENT(eEventType::PostRender);
		}

		// present
		{
			m_pGraphics->GetSwapChain()->Present(m_pGraphics->IsVSyncEnabled() ? 1 : 0, 0);
		}

		m_FrameCount++;
	}

	void Renderer::ResizeTargets(uint32_t width, uint32_t height)
	{
		if (m_Width == width && m_Height == height)
			return;

		m_Width = width;
		m_Height = height;

		// GBuffer
		{
			m_RenderTargets[static_cast<size_t>(eRenderTargetType::GBuffer_Diffuse)] = std::make_unique<Texture2D>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, false);
			m_RenderTargets[static_cast<size_t>(eRenderTargetType::GBuffer_Normal)] = std::make_unique<Texture2D>(width, height, DXGI_FORMAT_R11G11B10_FLOAT, false);
			m_RenderTargets[static_cast<size_t>(eRenderTargetType::GBuffer_Specular)] = std::make_unique<Texture2D>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, false);
			m_RenderTargets[static_cast<size_t>(eRenderTargetType::GBuffer_DepthStencil)] = std::make_unique<Texture2D>(width, height, 24, true);
		}

		// 기타
		{
			m_RenderTargets[static_cast<size_t>(eRenderTargetType::FrameRender)] = std::make_unique<Texture2D>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, false);
		}

		DV_LOG(Renerer, info, "Resize targets - {0:d} x {1:d}", m_Width, m_Height);
	}

	ID3D11RenderTargetView* Renderer::GetRenderTargetView(eRenderTargetType type)
	{
		auto& pRenderTarget = m_RenderTargets[static_cast<size_t>(type)];
		if (pRenderTarget)
		{
			return pRenderTarget->GetRenderTargetView();
		}

		return nullptr;
	}

	ID3D11ShaderResourceView* Renderer::GetShaderResourceView(eRenderTargetType type)
	{
		auto& pRenderTarget = m_RenderTargets[static_cast<size_t>(type)];
		if (pRenderTarget)
		{
			return pRenderTarget->GetShaderResourceView();
		}

		return nullptr;
	}
}
