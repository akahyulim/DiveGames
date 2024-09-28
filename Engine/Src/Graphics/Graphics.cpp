#include "divepch.h"
#include "Graphics.h"
#include "ConstantBuffer.h"
#include "Texture2D.h"
#include "RenderTexture.h"
#include "DvTexture2D.h"
#include "DvTexture2DArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	Graphics* Graphics::s_pInstance = nullptr;
	constexpr LPCWSTR DV_WINCLASS_NAME = L"DIVE_WINDOW";

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = NULL;

		switch (msg)
		{
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return Graphics::GetInstance()->MessageHandler(hWnd, msg, wParam, lParam);
	}

	Graphics::Graphics()
		: m_hInstance{}
		, m_hWnd{}
		, m_WindowTitle(L"DiveGames")
		, m_bFullScreen{}
		, m_ResolutionWidth{}
		, m_ResolutionHeight{}
		, m_pSwapChain{}
		, m_pDevice{}
		, m_pDeviceContext{}
		, m_pDefaultRenderTargetView{}
		, m_pDefaultDepthStencilView{}
		, m_pDefaultDepthTexture{}
		, m_bVSync{}
		, m_RenderTargetViews{}
		, m_pDepthStencilView{}
		, m_bRenderTargetDirty(true)
		, m_DepthStencilStates{}
		, m_DepthStencilStateType(eDepthStencilStateType::Count)
		, m_RasterizerStates{}
		, m_RasterizerStateType(eRasterizerStateType::Count)
		, m_BlendStates{}
		, m_BlendStateType(eBlendStateType::Count)
		, m_Viewport{}
		, m_VSConstBufs{}
		, m_VSConstBufSlots{}
		, m_bVSConstBufDirty(true)
		, m_DSConstBufs{}
		, m_DSConstBufSlots{}
		, m_bDSConstBufDirty(true)
		, m_GSConstBufs{}
		, m_GSConstBufSlots{}
		, m_bGSConstBufDirty(true)
		, m_PSConstBufs{}
		, m_PSConstBufSlots{}
		, m_bPSConstBufDirty(true)
		, m_PSResourceViews{}
		, m_bPSResourceViewDirty(true)
		, m_PSResourceViewDirtyFirst{}
		, m_PSResourceViewDirtyLast{}
		, m_PSSamplers{}
		, m_bPSSamplerDirty(true)
		, m_PSSamplerDirtyFirst{}
		, m_PSSamplerDirtyLast{}
		, m_pVertexBuffer{}
		, m_pIndexBuffer{}
		, m_PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED)
	{
		for (uint8_t i = 0; i != static_cast<uint8_t>(eVertexShaderType::Count); ++i)
			m_VertexShaders[i] = nullptr;
		m_VertexShaderType = eVertexShaderType::Count;	// null�� �ص� ������ ���� �� ������...

		for (uint8_t i = 0; i != static_cast<uint8_t>(ePixelShaderType::Count); ++i)
			m_PixelShaders[i] = nullptr;
		m_PixelShaderType = ePixelShaderType::Count;
	}

	Graphics::~Graphics()
	{
		Shutdown();
	}

	bool Graphics::Initialize(uint32_t width, uint32_t height, bool fullScreen, bool borderless)
	{
		// ������ ����
		if (!m_hWnd)
		{
			if (!createWindow(width, height, borderless))
				return false;
		}
		AdjustWindow(width, height, borderless);

		// ����̽� & ����ü�� ����
		if (!m_pDevice)
		{
			if (!createDevice(width, height))
				return false;
		}
		updateSwapChain(width, height);

		if (!createDepthStencilStates())
			return false;

		if (!createRasterizerStates())
			return false;

		if (!createBlendStates())
			return false;

		if (!createShaders())
			return false;

		ClearViews(eClearFlags::Color, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
		m_pSwapChain->Present(0, 0);

		ShowWindow(m_hWnd, SW_SHOW);
		SetFocus(m_hWnd);

		DV_ENGINE_TRACE("�׷��Ƚ� �ý��� �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void Graphics::Shutdown()
	{
		DV_RELEASE(m_pDefaultDepthTexture);
		DV_RELEASE(m_pDefaultDepthStencilView);
		DV_RELEASE(m_pDefaultRenderTargetView);

		DV_RELEASE(m_pDeviceContext);
		DV_RELEASE(m_pDeviceContext);
		DV_RELEASE(m_pSwapChain);

		::DestroyWindow(m_hWnd);
		::UnregisterClassW(DV_WINCLASS_NAME, m_hInstance);

		DV_ENGINE_TRACE("�׷��Ƚ� �ý��� �˴ٿ �����Ͽ����ϴ�.");
	}

	bool Graphics::RunWindow()
	{
		DV_ENGINE_ASSERT(IsInitialized());

		MSG msg{};
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return msg.message != WM_QUIT;
	}

	LRESULT Graphics::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_SIZE:
			if (IsInitialized())
				updateSwapChain(0, 0);
			return 0;
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void Graphics::AdjustWindow(uint32_t width, uint32_t height, bool borderless)
	{
		if (!m_hWnd)
		{
			DV_ENGINE_WARN("������ �����찡 �������� �ʽ��ϴ�.");
			return;
		}

		DWORD style = borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

		auto curStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
		if (curStyle != style)
			::SetWindowLongPtr(m_hWnd, GWL_STYLE, style);

		RECT rt = { 0, 0, (LONG)width, (LONG)height };
		::AdjustWindowRect(&rt, style, FALSE);

		width = rt.right - rt.left;
		height = rt.bottom - rt.top;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		::SetWindowPos(m_hWnd, NULL, posX, posY, width, height, SWP_DRAWFRAME);

		::GetWindowRect(m_hWnd, &rt);
		DV_ENGINE_INFO("WindowRect size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
		::GetClientRect(m_hWnd, &rt);
		DV_ENGINE_INFO("ClientRect size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		ShowWindow(m_hWnd, SW_SHOW);
	}

	bool Graphics::IsInitialized()
	{
		return m_pDevice != nullptr;
	}

	void Graphics::SetWindowTitle(const std::wstring& title)
	{
		if (!m_hWnd)
			return;

		if (m_WindowTitle != title)
		{
			SetWindowText(m_hWnd, title.c_str());
			m_WindowTitle = title;
		}
	}

	void Graphics::SetFullScreen(bool enabled)
	{
		if (!m_pSwapChain)
			return;

		if (m_bFullScreen != enabled)
		{
			m_pSwapChain->SetFullscreenState(enabled, nullptr);
			m_bFullScreen = enabled;
		}

		RECT rt{};
		GetClientRect(m_hWnd, &rt);
		DV_ENGINE_INFO("ClientRect size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
	}

	void Graphics::ResizeResolution(uint32_t width, uint32_t height)
	{
		if (!m_pSwapChain)
			return;

		DXGI_MODE_DESC desc{};
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.RefreshRate.Denominator = 1;		// �ϵ� �ڵ�
		desc.RefreshRate.Numerator = 60;		// �ϵ� �ڵ�
		desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		if (FAILED(m_pSwapChain->ResizeTarget(&desc)))
		{
			DV_ENGINE_ERROR("����ü�� Ÿ�� ������� �����Ͽ� ������ ũ�⸦ ������ �� �����ϴ�.");
			return;
		}
	}

	bool Graphics::BeginFrame()
	{
		if (!IsInitialized())
			return false;

		resetViews();

		// Count�� constexptr�� �ϴ°� ���ƺ��δ�?
		//for (uint32_t i = 0; i < (uint32_t)eTextureUnitType::Count; i++)
		//	BindPSResource(nullptr, i);

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		m_pSwapChain->Present(m_bVSync ? 1 : 0, 0);
	}

	bool Graphics::BindRenderTargetView(ID3D11RenderTargetView* pRtv, uint8_t index)
	{
		if (index >= MAX_NUM_RENDER_VIEWS)
		{
			DV_ENGINE_ERROR("�߸��� ���� Ÿ�� ���� ���� �ε���({:d})�� ���޹޾� ���ε��� �����Ͽ����ϴ�.", index);
			return false;
		}

		if (pRtv != m_RenderTargetViews[index])
		{
			m_RenderTargetViews[index] = pRtv;
			m_bRenderTargetDirty = true;
		}

		return true;
	}

	// �̰� BindTextureToRenderTargetView()ó�� �̸��� ��üȭ�ϴ� ���� ���� �� ����.
	bool Graphics::BindRenderTargetView(RenderTexture* pTexture, uint8_t index)
	{
		ID3D11RenderTargetView* pRtv{};
		if (pTexture)
			pRtv = pTexture->GetRenderTargetView();

		return BindRenderTargetView(pRtv, index);
	}

	bool Graphics::BindRenderTargetView(DvTexture2D* pTexture, uint8_t index)
	{
		ID3D11RenderTargetView* pRtv{};
		if (pTexture)
			pRtv = pTexture->GetRenderTargetView();

		//return BindRenderTargetView(pRtv, index);
		// �Ʒ��� �ڵ尡 �� ����� ��?
		return BindRenderTargetView(pTexture ? pTexture->GetRenderTargetView() : nullptr, index);
	}

	void Graphics::BindDepthStencilView(ID3D11DepthStencilView* pDsv)
	{
		if (pDsv != m_pDepthStencilView)
		{
			m_pDepthStencilView = pDsv;
			m_bRenderTargetDirty = true;
		}
	}

	void Graphics::BindDepthStencilView(RenderTexture* pTexture)
	{
		BindDepthStencilView(pTexture ? pTexture->GetDepthStencilView() : nullptr);
	}

	void Graphics::BindDepthStencilView(DvTexture2D* pTexture)
	{

		BindDepthStencilView(pTexture ? pTexture->GetDepthStencilView() : nullptr);
	}

	void Graphics::ClearViews(uint8_t flags, const DirectX::XMFLOAT4& color, float depth, uint8_t stencil)
	{
		// urho�� �̿� ����ϰ� PrepareDraw()�� ȣ���ϴ� ���� ������
		// �� ������ PrepareDraw()�� ���Ѵ�.
		if (m_bRenderTargetDirty)
		{
			m_pDeviceContext->OMSetRenderTargets(MAX_NUM_RENDER_VIEWS, m_RenderTargetViews, m_pDepthStencilView);
			m_bRenderTargetDirty = false;
		}

		if (flags & eClearFlags::Color)
		{
			float clearColor[4] = { color.x, color.y, color.z, color.w };

			for (uint32_t i = 0; i < MAX_NUM_RENDER_VIEWS; ++i)
			{
				if (m_RenderTargetViews[i])
				{
					m_pDeviceContext->ClearRenderTargetView(m_RenderTargetViews[i], clearColor);
				}
			}
		}

		if ((flags & eClearFlags::Depth) || (flags & eClearFlags::Stencil))
		{
			if (m_pDepthStencilView)
			{
				uint32_t clearFlags = 0;
				clearFlags |= (flags & eClearFlags::Depth) ? D3D11_CLEAR_DEPTH : 0;
				clearFlags |= (flags & eClearFlags::Stencil) ? D3D11_CLEAR_STENCIL : 0;


				m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, clearFlags, depth, stencil);
			}
		}
	}

	void Graphics::SetDepthStencilState(eDepthStencilStateType type, uint32_t stencilRef)
	{
		if (type != m_DepthStencilStateType)
		{
			auto index = static_cast<uint8_t>(type);
			m_pDeviceContext->OMSetDepthStencilState(m_DepthStencilStates[index], stencilRef);
			m_DepthStencilStateType = type;
		}
	}

	void Graphics::SetRasterizerState(eRasterizerStateType type)
	{
		if (type != m_RasterizerStateType)
		{
			// �ӽ�
			if (type == eRasterizerStateType::Count)
			{
				m_pDeviceContext->RSSetState(nullptr);
				return;
			}
			auto index = static_cast<uint8_t>(type);
			m_pDeviceContext->RSSetState(m_RasterizerStates[index]);
			m_RasterizerStateType =type;
		}
	}
	
	void Graphics::SetBlendState(eBlendStateType type)
	{
		if (type != m_BlendStateType)
		{
			auto index = static_cast<uint8_t>(type);
			float factor[4]{};
			uint32_t mask = 0xffff;
			m_pDeviceContext->OMSetBlendState(m_BlendStates[index], factor, mask);
			m_BlendStateType = type;
		}
	}

	void Graphics::SetViewport(D3D11_VIEWPORT viewport)
	{
		if (m_Viewport.TopLeftX != viewport.TopLeftX || m_Viewport.TopLeftY != viewport.TopLeftY || m_Viewport.Width != viewport.Width
			|| m_Viewport.Height != viewport.Height || m_Viewport.MinDepth != viewport.MinDepth || m_Viewport.MaxDepth != viewport.MaxDepth)
		{
			m_pDeviceContext->RSSetViewports(1, &viewport);
			m_Viewport = viewport;
		}
	}

	void Graphics::SetViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
	{
		D3D11_VIEWPORT viewport = { topLeftX, topLeftY, width, height, minDepth, maxDepth };
		SetViewport(viewport);
	}

	void Graphics::SetViewports(uint32_t count, const D3D11_VIEWPORT* pViewports)
	{
		m_pDeviceContext->RSSetViewports(count, pViewports);
	}

	void Graphics::SetVertexBuffer(VertexBuffer* pBuffer)
	{
		if (pBuffer != m_pVertexBuffer)
		{
			if (pBuffer)
			{
				ID3D11Buffer* pBuf = pBuffer->GetBuffer();
				uint32_t stride = pBuffer->GetStride();
				uint32_t offset = 0;

				m_pDeviceContext->IASetVertexBuffers(0, 1, &pBuf, &stride, &offset);
			}
			else
				m_pDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

			m_pVertexBuffer = pBuffer;
		}
	}

	void Graphics::SetIndexBuffer(IndexBuffer* pBuffer)
	{
		if (pBuffer != m_pIndexBuffer)
		{
			if(pBuffer)
				m_pDeviceContext->IASetIndexBuffer(pBuffer->GetBuffer(), pBuffer->GetFormat(), 0);
			else
				m_pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

			m_pIndexBuffer = pBuffer;
		}
	}

	void Graphics::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		if (topology != m_PrimitiveTopology)
		{
			m_pDeviceContext->IASetPrimitiveTopology(topology);
			m_PrimitiveTopology = topology;
		}
	}
	
	void Graphics::SetVertexShader(eVertexShaderType type)
	{
		if (type != m_VertexShaderType)
		{
			auto index = static_cast<uint8_t>(type);
			auto pShader = m_VertexShaders[index];
			m_pDeviceContext->IASetInputLayout(pShader ? pShader->GetInputLayout() : nullptr);
			m_pDeviceContext->VSSetShader(pShader ? pShader->GetVertexShader() : nullptr, nullptr, 0);

			m_VertexShaderType = type;
		}
	}

	void Graphics::SetHullShader(eHullShaderType type)
	{
		if (type != m_HullShaderType)
		{
			auto index = static_cast<uint8_t>(type);
			auto pShader = m_HullShaders[index];
			m_pDeviceContext->HSSetShader(pShader ? pShader->GetHullShader() : nullptr, nullptr, 0);

			m_HullShaderType = type;
		}
	}

	void Graphics::SetDomainShader(eDomainShaderType type)
	{
		if (type != m_DomainShaderType)
		{
			auto index = static_cast<uint8_t>(type);
			auto pShader = m_DomainShaders[index];
			m_pDeviceContext->DSSetShader(pShader ? pShader->GetDomainShader() : nullptr, nullptr, 0);

			m_DomainShaderType = type;
		}
	}

	void Graphics::SetGeometryShader(eGeometryShaderType type)
	{
		if (type != m_GeometryShaderType)
		{
			auto index = static_cast<uint8_t>(type);
			auto pShader = m_GeometryShaders[index];
			m_pDeviceContext->GSSetShader(pShader ? pShader->GetGeometryShader() : nullptr, nullptr, 0);

			m_GeometryShaderType = type;
		}
	}

	void Graphics::SetPixelShader(ePixelShaderType type)
	{
		if (type != m_PixelShaderType)
		{
			auto index = static_cast<uint8_t>(type);
			auto pShader = m_PixelShaders[index];
			m_pDeviceContext->PSSetShader(pShader ? pShader->GetPixelShader() : nullptr, nullptr, 0);

			m_PixelShaderType = type;
		}
	}

	// �̸��� �ٽ� BindConstantBufferVS�� �ٲ���.
	void Graphics::VSBindConstantBuffer(ConstantBuffer* pBuffer, size_t slot)
	{
		if (pBuffer != m_VSConstBufs[slot])
		{
			m_VSConstBufs[slot] = pBuffer;
			m_bVSConstBufDirty = true;
		}
	}

	void Graphics::DSBindConstantBuffer(ConstantBuffer* pBuffer, size_t slot)
	{
		if (pBuffer != m_DSConstBufs[slot])
		{
			m_DSConstBufs[slot] = pBuffer;
			m_bDSConstBufDirty = true;
		}
	}

	void Graphics::GSBindConstantBuffer(ConstantBuffer* pBuffer, size_t slot)
	{
		if (pBuffer != m_GSConstBufs[slot])
		{
			m_GSConstBufs[slot] = pBuffer;
			m_bGSConstBufDirty = true;
		}
	}

	void Graphics::PSBindConstantBuffer(ConstantBuffer* pBuffer, size_t slot)
	{
		if (pBuffer != m_PSConstBufs[slot])
		{
			m_PSConstBufs[slot] = pBuffer;
			m_bPSConstBufDirty = true;
		}
	}

	void Graphics::BindPSResource(ID3D11ShaderResourceView* pResourceView, eTextureUnitType unit)
	{
		if (unit >= eTextureUnitType::Count)
		{
			DV_ENGINE_ERROR("�߸��� �ȼ� ���̴� ���ҽ� ���� �ε���({:d})�� ���޹޾� ���ε��� �����Ͽ����ϴ�.", static_cast<uint8_t>(unit));
			return;
		}

		uint8_t index = static_cast<uint8_t>(unit);
		if (m_PSResourceViews[index] != pResourceView)
		{
			if (m_PSResourceViewDirtyFirst == MAX_NUM_SHADER_RESOURCES)
				m_PSResourceViewDirtyFirst = m_PSResourceViewDirtyLast = index;
			else
			{
				if (index < m_PSResourceViewDirtyFirst)
					m_PSResourceViewDirtyFirst = index;
				if (index > m_PSResourceViewDirtyLast)
					m_PSResourceViewDirtyLast = index;
			}

			m_PSResourceViews[index] = pResourceView;
			m_bPSResourceViewDirty = true;
		}
	}

	// �Ʒ� �� ���� �޼���� �����ϴ� ���� ����???
	// �㳪 ���� ����ϴ� �ڵ尡 �ִ�.
	void Graphics::BindPSResource(Texture* pTexture, eTextureUnitType unit)
	{
		BindPSResource(pTexture ? pTexture->GetShaderResourceView() : nullptr, unit);
	}

	void Graphics::BindPSResource(Texture2D* pTexture, eTextureUnitType unit)
	{
		BindPSResource(pTexture ? pTexture->GetShaderResourceView() : nullptr, unit);
	}

	void Graphics::BindPSResource(RenderTexture* pRenderTexture, eTextureUnitType unit)
	{
		BindPSResource(pRenderTexture ? pRenderTexture->GetShaderResourceView() : nullptr, unit);
	}

	void Graphics::BindPSResource(DvTexture* pTexture, eTextureUnitType unit)
	{
		BindPSResource(pTexture ? pTexture->GetShaderResourceView() : nullptr, unit);
	}

	// ���� ���̽��� �����ϴ� �޼��尡 �ֱ� ������ ���ʿ��ϴ�.
	void Graphics::BindPSResource(DvTexture2D* pTexture, eTextureUnitType unit)
	{
		BindPSResource(pTexture ? pTexture->GetShaderResourceView() : nullptr, unit);
	}

	// size_t�� eSamplerType�� �ٲپ�� �� ��
	// ���Ŀ� SamplerState�� ���� Ŭ������ ����� ���ο��� ���� �ε����� �����ϴ� ���� ���� ����...
	void Graphics::BindPSSampler(ID3D11SamplerState* pSampler, uint8_t index)
	{
		if (pSampler != m_PSSamplers[index])
		{
			if (m_PSSamplerDirtyFirst == MAX_NUM_SHADER_RESOURCES)
				m_PSSamplerDirtyFirst = m_PSSamplerDirtyLast = index;
			else
			{
				if (index < m_PSSamplerDirtyFirst)
					m_PSSamplerDirtyFirst = index;
				if (index > m_PSSamplerDirtyLast)
					m_PSSamplerDirtyLast = index;
			}

			m_PSSamplers[index] = pSampler;
			m_bPSSamplerDirty = true;
		}
	}

	void Graphics::BindPSSampler(uint8_t index, ID3D11SamplerState* pSampler)
	{
		if (index >= static_cast<uint8_t>(eSamplerType::Count))
		{
			DV_ENGINE_ERROR("");
			return;
		}

		if (pSampler != m_PSSamplers[index])
		{
			if (m_PSSamplerDirtyFirst == MAX_NUM_SHADER_RESOURCES)
				m_PSSamplerDirtyFirst = m_PSSamplerDirtyLast = index;
			else
			{
				if (index < m_PSSamplerDirtyFirst)
					m_PSSamplerDirtyFirst = index;
				if (index > m_PSSamplerDirtyLast)
					m_PSSamplerDirtyLast = index;
			}

			m_PSSamplers[index] = pSampler;
			m_bPSSamplerDirty = true;
		}
	}

	void Graphics::SetBoundResources()
	{
		if (m_bVSConstBufDirty)
		{
			ID3D11Buffer* buffers[MAX_NUM_VS_CONSTANT_BUFFERS]{};
			for (uint8_t i = 0; i != MAX_NUM_VS_CONSTANT_BUFFERS; ++i)
				buffers[i] = m_VSConstBufs[i] ? m_VSConstBufs[i]->GetBuffer() : nullptr;

			m_pDeviceContext->VSSetConstantBuffers(0, MAX_NUM_VS_CONSTANT_BUFFERS, buffers);
			m_bVSConstBufDirty = false;
		}

		if (m_bDSConstBufDirty)
		{
			ID3D11Buffer* buffers[MAX_NUM_DS_CONSTANT_BUFFERS]{};
			for (uint8_t i = 0; i != MAX_NUM_DS_CONSTANT_BUFFERS; ++i)
				buffers[i] = m_DSConstBufs[i] ? m_DSConstBufs[i]->GetBuffer() : nullptr;

			m_pDeviceContext->DSSetConstantBuffers(0, MAX_NUM_DS_CONSTANT_BUFFERS, buffers);
			m_bDSConstBufDirty = false;
		}

		if (m_bGSConstBufDirty)
		{
			ID3D11Buffer* buffers[MAX_NUM_GS_CONSTANT_BUFFERS]{};
			for (uint8_t i = 0; i != MAX_NUM_GS_CONSTANT_BUFFERS; ++i)
				buffers[i] = m_GSConstBufs[i] ? m_GSConstBufs[i]->GetBuffer() : nullptr;

			m_pDeviceContext->GSSetConstantBuffers(0, MAX_NUM_GS_CONSTANT_BUFFERS, buffers);
			m_bGSConstBufDirty = false;
		}

		if (m_bPSConstBufDirty)
		{
			ID3D11Buffer* buffers[MAX_NUM_PS_CONSTANT_BUFFERS]{};
			for (uint8_t i = 0; i != MAX_NUM_PS_CONSTANT_BUFFERS; ++i)
				buffers[i] = m_PSConstBufs[i] ? m_PSConstBufs[i]->GetBuffer() : nullptr;
			
			m_pDeviceContext->PSSetConstantBuffers(0, MAX_NUM_PS_CONSTANT_BUFFERS, buffers);
			m_bPSConstBufDirty = false;
		}

		if (m_bPSResourceViewDirty)
		{
			m_pDeviceContext->PSSetShaderResources(
				m_PSResourceViewDirtyFirst,
				m_PSResourceViewDirtyLast - m_PSResourceViewDirtyFirst + 1,
				&m_PSResourceViews[m_PSResourceViewDirtyFirst]);
			m_bPSResourceViewDirty = false;
		}

		if (m_bPSSamplerDirty)
		{
			m_pDeviceContext->PSSetSamplers(
				m_PSSamplerDirtyFirst,
				m_PSSamplerDirtyLast - m_PSSamplerDirtyFirst + 1,
				&m_PSSamplers[m_PSSamplerDirtyFirst]);
			m_bPSSamplerDirty = false;
		}
	}

	void Graphics::Draw(uint32_t vertexCount, uint32_t vertexStart)
	{
		SetBoundResources();

		m_pDeviceContext->Draw(vertexCount, vertexStart);
	}

	void Graphics::DrawIndexed(uint32_t indexCount, uint32_t indexStart, uint32_t vertexStart)
	{
		SetBoundResources();

		m_pDeviceContext->DrawIndexed(indexCount, indexStart, vertexStart);
	}

	bool Graphics::createWindow(uint32_t width, uint32_t height, bool borderless)
	{
		m_hInstance = GetModuleHandle(NULL);

		WNDCLASSEX wc{};
		wc.style = 0;
		wc.hInstance = m_hInstance;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = DV_WINCLASS_NAME;
		wc.cbSize = sizeof(WNDCLASSEX);

		if (!RegisterClassEx(&wc))
		{
			DV_ENGINE_ERROR("������Ŭ���� ��Ͽ� �����Ͽ����ϴ�.");
			return false;
		}

		DWORD style = borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		m_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			DV_WINCLASS_NAME,
			m_WindowTitle.c_str(),
			style,
			posX, posY,
			width, height,
			NULL, NULL,
			m_hInstance,
			NULL);

		if (!m_hWnd)
		{
			DV_ENGINE_ERROR("������ ������ �����Ͽ����ϴ�.");
			return false;
		}

		SetForegroundWindow(m_hWnd);

		return true;
	}

	bool Graphics::createDevice(uint32_t width, uint32_t height)
	{
		if (!m_pDevice)
		{
			//D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

			if (FAILED(D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				0,
				nullptr, //&featureLevel,//nullptr,
				0, //1, //0,
				D3D11_SDK_VERSION,
				&m_pDevice,
				nullptr,
				&m_pDeviceContext)))
			{
				DV_RELEASE(m_pDevice);
				DV_RELEASE(m_pDeviceContext);
				DV_ENGINE_ERROR("D3D11 ��ġ ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		if (m_pSwapChain)
			DV_RELEASE(m_pSwapChain);

		IDXGIDevice* pDxgiDevice{};
		m_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter{};
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory{};
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		DXGI_SWAP_CHAIN_DESC desc{};
		desc.BufferCount = 1;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB ���� ���ο� ���� �޶�����.
		desc.BufferDesc.RefreshRate.Denominator = 1;			// ���� ����(vsync�� ���� �޶�����?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// ��Ƽ ���ø� off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = !m_bFullScreen;
		desc.OutputWindow = GetWindowHandle();
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek���� 0�̰� �ٸ� ���� ������ ���� �ִ�...

		if (FAILED(pDxgiFactory->CreateSwapChain(m_pDevice, &desc, &m_pSwapChain)))
		{
			DV_RELEASE(m_pSwapChain);
			DV_ENGINE_ERROR("D3D11 ����ü�� ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}

	bool Graphics::updateSwapChain(uint32_t width, uint32_t height)
	{
		if (m_ResolutionWidth == width && m_ResolutionHeight == height)
			return true;

		m_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		DV_RELEASE(m_pDefaultRenderTargetView);
		DV_RELEASE(m_pDefaultDepthTexture);
		DV_RELEASE(m_pDefaultDepthStencilView);

		m_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		ID3D11Texture2D* pBackbufferTexture{};
		if (FAILED(m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_ENGINE_ERROR("�ĸ� ���� �ؽ��ĸ� �����µ� �����Ͽ����ϴ�.");
			return false;
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), nullptr, &m_pDefaultRenderTargetView)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_RELEASE(m_pDefaultRenderTargetView);
			DV_ENGINE_ERROR("�ĸ� ���� ����Ÿ�ٺ� ������ �����Ͽ����ϴ�.");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		uint32_t curWidth = width;
		uint32_t curHeight = height;

		if (width == 0 || height == 0)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			m_pSwapChain->GetDesc(&swapChainDesc);

			curWidth = swapChainDesc.BufferDesc.Width;
			curHeight = swapChainDesc.BufferDesc.Height;
		}

		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = curWidth;
		texDesc.Height = curHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;// static_cast<UINT>(screenParamm_.multiSample_);
		texDesc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(texDesc.Format, screenParamm_.multiSample_);
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pDefaultDepthTexture)))
		{
			DV_RELEASE(m_pDefaultDepthTexture);
			DV_ENGINE_ERROR("�ĸ� ���� ���� ���ٽ� �ؽ��� ������ �����Ͽ����ϴ�.");
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pDevice->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(m_pDefaultDepthTexture),
			&viewDesc,//nullptr,		// urho�� nullptr�� �����ߴ�. ���ó� sampler ������ �ذ���� �ʾҴ�.
			&m_pDefaultDepthStencilView)))
		{
			DV_RELEASE(m_pDefaultDepthStencilView);
			DV_ENGINE_ERROR("�ĸ� ���� ���� ���ٽ� �� ������ �����Ͽ����ϴ�.");
			return false;
		}

		m_ResolutionWidth = curWidth;
		m_ResolutionHeight = curHeight;

		RECT rt{};
		GetClientRect(m_hWnd, &rt);
		DV_ENGINE_ASSERT(m_ResolutionWidth == (rt.right - rt.left));
		DV_ENGINE_ASSERT(m_ResolutionHeight == (rt.bottom - rt.top));
		DV_ENGINE_INFO("Resolution: {0:d} x {1:d}", m_ResolutionWidth, m_ResolutionHeight);

		resetViews();

		return true;
	}

	// beginFrame(), reiszeBackbuffer() ��� ȣ��
	void Graphics::resetViews()
	{
		for (uint8_t i = 0; i != MAX_NUM_RENDER_VIEWS; ++i)
			BindRenderTargetView(static_cast<ID3D11RenderTargetView*>(nullptr), i);
		BindDepthStencilView(static_cast<ID3D11DepthStencilView*>(nullptr));

		SetViewport(0.0f, 0.0f, static_cast<float>(m_ResolutionWidth), static_cast<float>(m_ResolutionHeight));
	}

	bool Graphics::createDepthStencilStates()
	{
		D3D11_DEPTH_STENCIL_DESC desc{};
		uint8_t index{};

		// Depth Less, No Stencil
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			
			index = static_cast<uint8_t>(eDepthStencilStateType::DepthLess);

			if (FAILED(m_pDevice->CreateDepthStencilState(&desc, &m_DepthStencilStates[index])))
			{
				DV_ENGINE_ERROR("DepthStencilState DepthLess ������ �����Ͽ����ϴ�.");
				return false;
			}
		}
		
		// Depth Equal, No Stencil
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			
			index = static_cast<uint8_t>(eDepthStencilStateType::DepthEqual);

			if (FAILED(m_pDevice->CreateDepthStencilState(&desc, &m_DepthStencilStates[index])))
			{
				DV_ENGINE_ERROR("DepthStencilState DepthEqual ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		// GBuffer - å ����. GBuffer�� ���� ���� �׽�Ʈ
		// ���� �׽�Ʈ�� ���̰��� �� ���� �� ����ϸ� ���ۿ� ��ϵȴ�.
		// ���ٽ� �׽�Ʈ�� �׻� ����ϸ� � ���� ���۷��� ������ ����ȴ�.
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC op = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
			desc.FrontFace = op;
			desc.BackFace = op;

			index = static_cast<uint8_t>(eDepthStencilStateType::GBuffer);

			if (FAILED(m_pDevice->CreateDepthStencilState(&desc, &m_DepthStencilStates[index])))
			{
				DV_ENGINE_ERROR("DepthStencilState GBuffer ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		// NoDepthWriteLessStencilMask - å ����. GBuffer �� Light�� ������ �� readOnly dsv�� ����Ͽ� ���� �׽�Ʈ
		// ���� �׽�Ʈ�� ���̰��� �� ���� �� ��������� ���ۿ��� ������� �ʴ´�. => ī�޶� �ʱٰŸ��� ��ǻ� �׻� �׽�Ʈ ���
		// ���ۿ� ���۷��� ���� ������ �� �׽�Ʈ�� ��������� ���۴� ���� ���� �����Ѵ�.
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	// read only
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC op = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
			desc.FrontFace = op;
			desc.BackFace = op;

			index = static_cast<uint8_t>(eDepthStencilStateType::NoDepthWriteLessStencilMask);

			if (FAILED(m_pDevice->CreateDepthStencilState(&desc, &m_DepthStencilStates[index])))
			{
				DV_ENGINE_ERROR("DepthStencilState NoDepthWriteLessStencilMask ������ �����Ͽ����ϴ�.");
				return false;
			}

			// NoDepthWriteGreaterStencilMask
			// ���� �׽�Ʈ�� ���̰��� �� ũ�ų� ���� �� ��������� ���ۿ��� ������� �ʴ´�.
			desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

			index = static_cast<uint8_t>(eDepthStencilStateType::NoDepthWriteGreaterStencilMask);

			if (FAILED(m_pDevice->CreateDepthStencilState(&desc, &m_DepthStencilStates[index])))
			{
				DV_ENGINE_ERROR("DepthStencilState NoDepthWriteGreaterStencilMask ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		// Depth Less / No Write, Stencil Mark DS

		// Depth Greater / No Write, Stencil Mask DS(ds�� ����?)

		// shadow gen
		{
			D3D11_DEPTH_STENCIL_DESC desc;
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC noSkyStencilOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
			desc.FrontFace = noSkyStencilOp;
			desc.BackFace = noSkyStencilOp;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.StencilEnable = FALSE;
			desc.DepthFunc = D3D11_COMPARISON_LESS;

			index = static_cast<uint8_t>(eDepthStencilStateType::ShadowGen);

			if (FAILED(m_pDevice->CreateDepthStencilState(&desc, &m_DepthStencilStates[index])))
			{
				DV_ENGINE_ERROR("DepthStencilState ShdowGen ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		return true;
	}
	
	bool Graphics::createRasterizerStates()
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

		uint8_t index = static_cast<uint8_t>(eRasterizerStateType::FillSolid_CullBack);

		if (FAILED(m_pDevice->CreateRasterizerState(&desc, &m_RasterizerStates[index])))
		{
			DV_ENGINE_ERROR("RasterizerState FillSolid_CullBack ������ �����Ͽ����ϴ�.");
			return false;
		}

		// FillSolid_CullNone
		desc.CullMode = D3D11_CULL_NONE;

		index = static_cast<uint8_t>(eRasterizerStateType::FillSolid_CullNone);

		if (FAILED(m_pDevice->CreateRasterizerState(&desc, &m_RasterizerStates[index])))
		{
			DV_ENGINE_ERROR("RasterizerState FillSolid_CullNode ������ �����Ͽ����ϴ�.");
			return false;
		}

		{
			// NoDepthClipFront
			D3D11_RASTERIZER_DESC desc = {
				D3D11_FILL_SOLID,
				D3D11_CULL_FRONT,
				FALSE,
				D3D11_DEFAULT_DEPTH_BIAS,
				D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
				D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
				TRUE,
				FALSE,
				FALSE,
				FALSE };

			index = static_cast<uint8_t>(eRasterizerStateType::NoDepthClipFront);

			if (FAILED(m_pDevice->CreateRasterizerState(&desc, &m_RasterizerStates[index])))
			{
				DV_ENGINE_ERROR("RasterizerState NoDepthClipFront ������ �����Ͽ����ϴ�.");
				return false;
			}

			// ShadowGen
			// cascaded �Ŀ� ������ �ʴ´�.
			desc.CullMode = D3D11_CULL_BACK;
			desc.DepthBias = 100;//85;
			desc.SlopeScaledDepthBias = 4.0f;//5.0f;
			desc.DepthBiasClamp = 0.0f;
			
			index = static_cast<uint8_t>(eRasterizerStateType::ShadowGen);

			if (FAILED(m_pDevice->CreateRasterizerState(&desc, &m_RasterizerStates[index])))
			{
				DV_ENGINE_ERROR("RasterizerState ShadowGen ������ �����Ͽ����ϴ�.");
				return false;
			}

			// Cascaded Shadow Gen
			desc  = {
			D3D11_FILL_SOLID,
			D3D11_CULL_FRONT,
			FALSE,
			D3D11_DEFAULT_DEPTH_BIAS,
			D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
			D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			TRUE,
			FALSE,
			FALSE,
			FALSE
			};
			desc.CullMode = D3D11_CULL_BACK;
			desc.FillMode = D3D11_FILL_SOLID;
			desc.DepthBias = 6;
			desc.SlopeScaledDepthBias = 1.0f;
			desc.DepthClipEnable = FALSE;

			index = static_cast<uint8_t>(eRasterizerStateType::CascadedShadowGen);

			if (FAILED(m_pDevice->CreateRasterizerState(&desc, &m_RasterizerStates[index])))
			{
				DV_ENGINE_ERROR("RasterizerState CascadedShadowGen ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		return true;
	}
	
	bool Graphics::createBlendStates()
	{
		D3D11_BLEND_DESC desc{};
		uint8_t index{};

		// Additive
		{
			ZeroMemory(&desc, sizeof(desc));
			desc.IndependentBlendEnable = FALSE;
			desc.AlphaToCoverageEnable = FALSE;
			desc.RenderTarget[0].BlendEnable = TRUE;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			index = static_cast<uint8_t>(eBlendStateType::Additive);

			if (FAILED(m_pDevice->CreateBlendState(&desc, &m_BlendStates[index])))
			{
				DV_ENGINE_ERROR("BlandState Additive ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		// Transparent
		{
			// ������ ��� ������ ���� ���ĸ� ���ϰ� ����� ���� ���� �������� ���� �� ���Ѵ�.
			ZeroMemory(&desc, sizeof(desc));
			//desc.AlphaToCoverageEnable = TRUE;
			desc.RenderTarget[0].BlendEnable = TRUE;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			index = static_cast<uint8_t>(eBlendStateType::Transparent);

			if (FAILED(m_pDevice->CreateBlendState(&desc, &m_BlendStates[index])))
			{
				DV_ENGINE_ERROR("BlandState Transparent ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		return true;
	}

	bool Graphics::createShaders()
	{
		// vertex shaders
		{
			auto index = static_cast<uint8_t>(eVertexShaderType::Null);
			m_VertexShaders[index] = nullptr;

			auto pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/LightDepth.hlsl", eShaderType::Vertex, eInputLayout::Pos))
				return false;
			index = static_cast<uint8_t>(eVertexShaderType::LightDepth);
			m_VertexShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DirLightDepth.hlsl", eShaderType::Vertex, eInputLayout::Pos))
				return false;
			index = static_cast<uint8_t>(eVertexShaderType::DirLightDepth);
			m_VertexShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/ForwardLight.hlsl", eShaderType::Vertex, eInputLayout::Static_Model))
				return false;
			index = static_cast<uint8_t>(eVertexShaderType::ForwardLight);
			m_VertexShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/GBuffer.hlsl", eShaderType::Vertex, eInputLayout::Static_Model))
				return false;
			index = static_cast<uint8_t>(eVertexShaderType::GBuffer);
			m_VertexShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredLight.hlsl", eShaderType::Vertex))//, eInputLayout::Pos))
				return false;
			index = static_cast<uint8_t>(eVertexShaderType::DeferredLight);
			m_VertexShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredDirLight.hlsl", eShaderType::Vertex))//, eInputLayout::None))
				return false;
			index = static_cast<uint8_t>(eVertexShaderType::DeferredDirLight);
			m_VertexShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredPointLight.hlsl", eShaderType::Vertex))//, eInputLayout::None))
				return false;
			index = static_cast<uint8_t>(eVertexShaderType::DeferredPointLight);
			m_VertexShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredSpotLight.hlsl", eShaderType::Vertex))
				return false;
			index = static_cast<uint8_t>(eVertexShaderType::DeferredSpotLight);
			m_VertexShaders[index] = pShader;
		}

		// hull shaders
		{
			auto index = static_cast<uint8_t>(eHullShaderType::Null);
			m_HullShaders[index] = nullptr;

			auto pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredPointLight.hlsl", eShaderType::Hull))
				return false;
			index = static_cast<uint8_t>(eHullShaderType::DeferredPointLight);
			m_HullShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredSpotLight.hlsl", eShaderType::Hull))
				return false;
			index = static_cast<uint8_t>(eHullShaderType::DeferredSpotLight);
			m_HullShaders[index] = pShader;
		}

		// domain shaders
		{
			auto index = static_cast<uint8_t>(eDomainShaderType::Null);
			m_DomainShaders[index] = nullptr;

			auto pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredPointLight.hlsl", eShaderType::Domain))
				return false;
			index = static_cast<uint8_t>(eDomainShaderType::DeferredPointLight);
			m_DomainShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredSpotLight.hlsl", eShaderType::Domain))
				return false;
			index = static_cast<uint8_t>(eDomainShaderType::DeferredSpotLight);
			m_DomainShaders[index] = pShader;
		}

		// geometry shaders
		{
			auto index = static_cast<uint8_t>(eGeometryShaderType::Null);
			m_GeometryShaders[index] = nullptr;

			auto pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DirLightDepth.hlsl", eShaderType::Geometry))
				return false;
			index = static_cast<uint8_t>(eGeometryShaderType::CascadeShadowMaps);
			m_GeometryShaders[index] = pShader;
		}

		// pixel shaders
		{
			auto index = static_cast<uint8_t>(ePixelShaderType::Null);
			m_PixelShaders[index] = nullptr;

			auto pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/ForwardLight.hlsl", eShaderType::Pixel))
				return false;
			index = static_cast<uint8_t>(ePixelShaderType::ForwardLight);
			m_PixelShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/GBuffer.hlsl", eShaderType::Pixel))
				return false;
			index = static_cast<uint8_t>(ePixelShaderType::GBuffer);
			m_PixelShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredLight.hlsl", eShaderType::Pixel))
				return false;
			index = static_cast<uint8_t>(ePixelShaderType::DeferredLight);
			m_PixelShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredDirLight.hlsl", eShaderType::Pixel))
				return false;
			index = static_cast<uint8_t>(ePixelShaderType::DeferredDirLight);
			m_PixelShaders[index] = pShader;

			pShader = new Shader(); 
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredPointLight.hlsl", eShaderType::Pixel))
				return false;
			index = static_cast<uint8_t>(ePixelShaderType::DeferredPointLight);
			m_PixelShaders[index] = pShader;

			pShader = new Shader();
			if (!pShader->CompileAndCreateShader("../../Assets/Shaders/DeferredSpotLight.hlsl", eShaderType::Pixel))
				return false;
			index = static_cast<uint8_t>(ePixelShaderType::DeferredSpotLight);
			m_PixelShaders[index] = pShader;
		}

		return true;
	}
}