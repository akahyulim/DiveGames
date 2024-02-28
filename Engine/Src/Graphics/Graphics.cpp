#include "divepch.h"
#include "Graphics.h"
#include "Shader.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	constexpr LPCWSTR DV_WINCLASS_NAME = L"DIVE_WINDOW";

	HINSTANCE Graphics::s_hInstance = NULL;
	HWND Graphics::s_hWnd = NULL;
	std::wstring Graphics::s_WindowTitle = L"DiveGames";
	bool Graphics::s_bFullScreen = false;
	uint32_t Graphics::s_ResolutionWidth = 0;
	uint32_t Graphics::s_ResolutionHeight = 0;

	IDXGISwapChain* Graphics::s_pSwapChain = nullptr;
	ID3D11Device* Graphics::s_pDevice = nullptr;
	ID3D11DeviceContext* Graphics::s_pDeviceContext = nullptr;

	ID3D11RenderTargetView* Graphics::s_pDefaultRenderTargetView = nullptr;
	ID3D11DepthStencilView* Graphics::s_pDefaultDepthStencilView = nullptr;
	ID3D11Texture2D* Graphics::s_pDefaultDepthTexture = nullptr;

	bool Graphics::s_bVSync = false;

	D3D11_PRIMITIVE_TOPOLOGY Graphics::s_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	ID3D11RenderTargetView* Graphics::s_RenderTargetViews[4] = { nullptr, };
	ID3D11DepthStencilView* Graphics::s_pDepthStencilView = nullptr;

	bool Graphics::s_bRenderTargetViewsDirty = true;

	Shader* Graphics::s_pShader = nullptr;

	VertexBuffer* Graphics::s_pVertexBuffer = nullptr;
	IndexBuffer* Graphics::s_pIndexBuffer = nullptr;

	ID3D11DepthStencilState* Graphics::s_pDepthStencilState = nullptr;
	ID3D11RasterizerState* Graphics::s_pRasterizerState = nullptr;
	ID3D11BlendState* Graphics::s_pBlendState = nullptr;

	ID3D11ShaderResourceView* Graphics::s_ShaderResourceViews[(uint32_t)eTextureUnit::Max_Num] = { nullptr, };
	ID3D11SamplerState* Graphics::s_SamplerStates[(uint32_t)eTextureUnit::Max_Num] = { nullptr, };

	bool Graphics::s_bTextureDirty = true;
	
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

		return Graphics::MessageHandler(hWnd, msg, wParam, lParam);
	}

	// urho�� fullScreen���� resizable���� ScreenModeParams�̶�� ����ü�� ����
	// vsync�� �ش� ����ü���� �����Ѵ�.
	bool Graphics::SetScreenMode(uint32_t width, uint32_t height, bool fullScreen, bool borderless)
	{
		// ������ ����
		if (!s_hWnd)
		{
			if (!createWindow(width, height, borderless))
				return false;
		}
		AdjustWindow(width, height, borderless);

		// ����̽� & ����ü�� ����
		if (!s_pDevice)
		{
			if (!createDevice(width, height))
				return false;
		}
		updateSwapChain(width, height);

		ClearViews(eClearFlags::Color, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
		s_pSwapChain->Present(0, 0);

		ShowWindow(s_hWnd, SW_SHOW);
		SetFocus(s_hWnd);

		return true;
	}

	void Graphics::Shutdown()
	{
		DV_RELEASE(s_pDefaultDepthTexture);
		DV_RELEASE(s_pDefaultDepthStencilView);
		DV_RELEASE(s_pDefaultRenderTargetView);

		DV_RELEASE(s_pDeviceContext);
		DV_RELEASE(s_pDeviceContext);
		DV_RELEASE(s_pSwapChain);

		::DestroyWindow(s_hWnd);
		::UnregisterClassW(DV_WINCLASS_NAME, s_hInstance);
	}

	bool Graphics::RunWindow()
	{
		DV_CORE_ASSERT(IsInitialized());
		
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));

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
			if(IsInitialized())
				updateSwapChain(0, 0);
			return 0;
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void Graphics::AdjustWindow(uint32_t width, uint32_t height, bool borderless)
	{
		if (!s_hWnd)
		{
			DV_CORE_WARN("������ �����찡 �������� �ʽ��ϴ�.");
			return;
		}

		DWORD style = borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

		auto curStyle = ::GetWindowLongPtr(s_hWnd, GWL_STYLE);
		if (curStyle != style)
			::SetWindowLongPtr(s_hWnd, GWL_STYLE, style);

		RECT rt = { 0, 0, (LONG)width, (LONG)height };
		::AdjustWindowRect(&rt, style, FALSE);

		width = rt.right - rt.left;
		height = rt.bottom - rt.top;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		::SetWindowPos(s_hWnd, NULL, posX, posY, width, height, SWP_DRAWFRAME);

		::GetWindowRect(s_hWnd, &rt);
		DV_CORE_INFO("WindowRect size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
		::GetClientRect(s_hWnd, &rt);
		DV_CORE_INFO("ClientRect size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

		ShowWindow(s_hWnd, SW_SHOW);
	}

	bool Graphics::IsInitialized()
	{
		return s_pDevice != nullptr;
	}

	void Graphics::SetWindowTitle(const std::wstring& title)
	{
		if (!s_hWnd)
			return;

		if (s_WindowTitle != title)
		{
			SetWindowText(s_hWnd, title.c_str());
			s_WindowTitle = title;
		}
	}

	void Graphics::SetFullScreen(bool enabled)
	{
		if (!s_pSwapChain)
			return;

		if (s_bFullScreen != enabled)
		{
			s_pSwapChain->SetFullscreenState(enabled, nullptr);
			s_bFullScreen = enabled;
		}

		RECT rt;
		GetClientRect(s_hWnd, &rt);
		DV_CORE_INFO("ClientRect size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);
	}

	void Graphics::ResizeResolution(uint32_t width, uint32_t height)
	{
		if (!s_pSwapChain)
			return;

		DXGI_MODE_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.RefreshRate.Denominator = 1;		// �ϵ� �ڵ�
		desc.RefreshRate.Numerator = 60;		// �ϵ� �ڵ�
		desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		if (FAILED(s_pSwapChain->ResizeTarget(&desc)))
		{
			DV_CORE_ERROR("����ü�� Ÿ�� ������� �����Ͽ� ������ ũ�⸦ ������ �� �����ϴ�.");
			return;
		}
	}

	bool Graphics::BeginFrame()
	{
		if(!IsInitialized())
			return false;

		resetViews();

		// Max_Num�� constexptr�� �ϴ°� ���ƺ��δ�?
		for (uint32_t i = 0; i < (uint32_t)eTextureUnit::Max_Num; i++)
			SetTexture((eTextureUnit)i, nullptr);

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		s_pSwapChain->Present(s_bVSync ? 1 : 0, 0);
	}

	void Graphics::ClearViews(uint8_t flags, const DirectX::XMFLOAT4& color, float depth, uint8_t stencil)
	{
		// �� ������ SetRenderTarget�� SetDepthStencil�� ��Ƽüũ �� ���ε��� �ϱ� �����̴�.
		// ���� �̸��� �� ��︮�� �ʰ� �Ǿ���.
		// ��¼�� �ش� �κ��� �и��ϴ� ���� �� �������� ���� �� ����.
		prepareDraw();

		float clearColor[4] = { color.x, color.y, color.z, color.w };

		if (flags & eClearFlags::Color)
			s_pDeviceContext->ClearRenderTargetView(s_RenderTargetViews[0], clearColor);

		if ((flags & eClearFlags::Depth) || (flags & eClearFlags::Stencil))
		{
			uint32_t clearFlags = 0;
			clearFlags |= (flags & eClearFlags::Depth) ? D3D11_CLEAR_DEPTH : 0;
			clearFlags |= (flags & eClearFlags::Stencil) ? D3D11_CLEAR_STENCIL : 0;

			s_pDeviceContext->ClearDepthStencilView(s_pDepthStencilView, clearFlags, depth, stencil);
		}
	}

	void Graphics::SetRenderTargetView(uint32_t index, ID3D11RenderTargetView* pRenderTargetView)
	{
		// ���� ����� constexpr�� �ٲٱ�
		if (index >= 4)
			return;

		if (pRenderTargetView != s_RenderTargetViews[index])
		{
			s_RenderTargetViews[index] = pRenderTargetView;
			s_bRenderTargetViewsDirty = true;
		}
	}

	// read only ������ ���� �ָ�������. 
	// RenderTexture�� �޾ƾ� �ϳ�...?
	void Graphics::SetDepthStencilView(ID3D11DepthStencilView* pDepthStencilView)
	{
		if (pDepthStencilView != s_pDepthStencilView)
		{
			s_pDepthStencilView = pDepthStencilView;
			s_bRenderTargetViewsDirty = true;
		}
	}

	void Graphics::SetViewport(const RECT& rt)
	{
		static D3D11_VIEWPORT viewport;
		viewport.TopLeftX = (float)rt.left;
		viewport.TopLeftY = (float)rt.top;
		viewport.Width = (float)(rt.right - rt.left);
		viewport.Height = (float)(rt.bottom - rt.top);
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 0.0f;

		s_pDeviceContext->RSSetViewports(1, &viewport);
	}

	void Graphics::SetDepthStencilState(ID3D11DepthStencilState* pState)
	{
		if (pState != s_pDepthStencilState)
		{
			s_pDeviceContext->OMSetDepthStencilState(pState, 0);
			s_pDepthStencilState = pState;
		}
	}

	void Graphics::SetRasterizerState(ID3D11RasterizerState* pState)
	{
		if (pState != s_pRasterizerState)
		{
			s_pDeviceContext->RSSetState(pState);
			s_pRasterizerState = pState;
		}
	}

	void Graphics::SetBlendState(ID3D11BlendState* pState)
	{
		if (pState != s_pBlendState)
		{
			float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			s_pDeviceContext->OMSetBlendState(pState, blendFactor, 0xffffffff);
			s_pBlendState = pState;
		}
	}

	void Graphics::SetTexture(eTextureUnit index, Texture* pTexture)
	{
		if (pTexture)
		{
			if (pTexture->GetShaderResourceView() != s_ShaderResourceViews[(uint32_t)index])
			{
				if (pTexture->IsMipLevelsDirty())
					pTexture->UpdateMipLevels();

				if (pTexture->IsSamplerStateDirty())
					pTexture->UpdateSamplerState();

				s_ShaderResourceViews[(uint32_t)index] = pTexture->GetShaderResourceView();
				s_SamplerStates[(uint32_t)index] = pTexture->GetSamplerState();

				s_bTextureDirty = true;
			}
		}
		else
		{
			if (s_ShaderResourceViews[(uint32_t)index])
			{
				s_ShaderResourceViews[(uint32_t)index] = nullptr;
				s_SamplerStates[(uint32_t)index] = nullptr;

				s_bTextureDirty = true;
			}
		}
	}

	void Graphics::SetShader(Shader* pShader)
	{
		if (pShader != s_pShader)
		{
			s_pDeviceContext->IASetInputLayout(pShader->GetInputLayout());
			s_pDeviceContext->VSSetShader(pShader->GetVertexShader(), nullptr, 0);
			s_pDeviceContext->PSSetShader(pShader->GetPixelShader(), nullptr, 0);

			s_pShader = pShader;
		}
	}

	// ���� ���۸� �ٷ�� �ִ�.
	void Graphics::SetVertexBuffer(VertexBuffer* pBuffer)
	{
		if (pBuffer != s_pVertexBuffer)
		{
			if (pBuffer)
				s_pDeviceContext->IASetVertexBuffers(0, 1, pBuffer->GetBuffer(), pBuffer->GetStride(), pBuffer->GetOffset());
			else
				s_pDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

			s_pVertexBuffer = pBuffer;
		}
	}

	void Graphics::SetIndexBuffer(IndexBuffer* pBuffer)
	{
		if (pBuffer != s_pIndexBuffer)
		{
			if (pBuffer)
				s_pDeviceContext->IASetIndexBuffer(pBuffer->GetBuffer(), pBuffer->GetFormat(), 0);
			else
				s_pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

			s_pIndexBuffer = pBuffer;
		}
	}

	void Graphics::Draw(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t vertexCount, uint32_t vertexStart)
	{
		prepareDraw();

		if (s_PrimitiveTopology != topology)
		{
			s_pDeviceContext->IASetPrimitiveTopology(topology);
			s_PrimitiveTopology = topology;
		}

		s_pDeviceContext->Draw(vertexCount, vertexStart);
	}

	void Graphics::DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t indexCount, uint32_t indexStart)
	{
		prepareDraw();

		if (s_PrimitiveTopology != topology)
		{
			s_pDeviceContext->IASetPrimitiveTopology(topology);
			s_PrimitiveTopology = topology;
		}

		s_pDeviceContext->DrawIndexed(indexCount, indexStart, 0);
	}

	bool Graphics::createWindow(uint32_t width, uint32_t height, bool borderless)
	{
		s_hInstance = GetModuleHandle(NULL);

		WNDCLASSEX wc;
		wc.style = 0;
		wc.hInstance = s_hInstance;
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
			DV_CORE_ERROR("������Ŭ���� ��Ͽ� �����Ͽ����ϴ�.");
			return false;
		}

		DWORD style = borderless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		s_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			DV_WINCLASS_NAME,
			s_WindowTitle.c_str(),
			style,
			posX, posY,
			width, height,
			NULL, NULL,
			s_hInstance,
			NULL);

		if (!s_hWnd)
		{
			DV_CORE_ERROR("������ ������ �����Ͽ����ϴ�.");
			return false;
		}

		SetForegroundWindow(s_hWnd);

		return true;
	}

	bool Graphics::createDevice(uint32_t width, uint32_t height)
	{
		if (!s_pDevice)
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
				&s_pDevice,
				nullptr,
				&s_pDeviceContext)))
			{
				DV_RELEASE(s_pDevice);
				DV_RELEASE(s_pDeviceContext);
				DV_CORE_ERROR("D3D11 ��ġ ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		if (s_pSwapChain)
			DV_RELEASE(s_pSwapChain);

		IDXGIDevice* pDxgiDevice = nullptr;
		s_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter = nullptr;
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory = nullptr;
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 1;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB ���� ���ο� ���� �޶�����.
		desc.BufferDesc.RefreshRate.Denominator = 1;			// ���� ����(vsync�� ���� �޶�����?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// ��Ƽ ���ø� off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = !s_bFullScreen;
		desc.OutputWindow = GetWindowHandle();
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek���� 0�̰� �ٸ� ���� ������ ���� �ִ�...

		if (FAILED(pDxgiFactory->CreateSwapChain(s_pDevice, &desc, &s_pSwapChain)))
		{
			DV_RELEASE(s_pSwapChain);
			DV_CORE_ERROR("D3D11 ����ü�� ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}

	bool Graphics::updateSwapChain(uint32_t width, uint32_t height)
	{
		if (s_ResolutionWidth == width && s_ResolutionHeight == height)
			return true;

		s_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		DV_RELEASE(s_pDefaultRenderTargetView);
		DV_RELEASE(s_pDefaultDepthTexture);
		DV_RELEASE(s_pDefaultDepthStencilView);

		s_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		ID3D11Texture2D* pBackbufferTexture = nullptr;
		if (FAILED(s_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_CORE_ERROR("�ĸ� ���� �ؽ��ĸ� �����µ� �����Ͽ����ϴ�.");
			return false;
		}

		if (FAILED(s_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), nullptr, &s_pDefaultRenderTargetView)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_RELEASE(s_pDefaultRenderTargetView);
			DV_CORE_ERROR("�ĸ� ���� ����Ÿ�ٺ� ������ �����Ͽ����ϴ�.");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		uint32_t curWidth = width;
		uint32_t curHeight = height;

		if (width == 0 || height == 0)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			s_pSwapChain->GetDesc(&swapChainDesc);

			curWidth = swapChainDesc.BufferDesc.Width;
			curHeight = swapChainDesc.BufferDesc.Height;
		}

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = curWidth;
		texDesc.Height = curHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;// static_cast<UINT>(screenParams_.multiSample_);
		texDesc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(texDesc.Format, screenParams_.multiSample_);
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (FAILED(s_pDevice->CreateTexture2D(&texDesc, nullptr, &s_pDefaultDepthTexture)))
		{
			DV_RELEASE(s_pDefaultDepthTexture);
			DV_CORE_ERROR("�ĸ� ���� ���� ���ٽ� �ؽ��� ������ �����Ͽ����ϴ�.");
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(viewDesc));
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(s_pDevice->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(s_pDefaultDepthTexture),
			&viewDesc,//nullptr,		// urho�� nullptr�� �����ߴ�. ���ó� sampler ������ �ذ���� �ʾҴ�.
			&s_pDefaultDepthStencilView)))
		{
			DV_RELEASE(s_pDefaultDepthStencilView);
			DV_CORE_ERROR("�ĸ� ���� ���� ���ٽ� �� ������ �����Ͽ����ϴ�.");
			return false;
		}

		s_ResolutionWidth = curWidth;
		s_ResolutionHeight = curHeight;

		RECT rt;
		GetClientRect(s_hWnd, &rt);
		DV_CORE_ASSERT(s_ResolutionWidth == (rt.right - rt.left));
		DV_CORE_ASSERT(s_ResolutionHeight == (rt.bottom - rt.top));
		DV_CORE_INFO("Resolution: {0:d} x {1:d}", s_ResolutionWidth, s_ResolutionHeight);

		resetViews();

		return true;
	}

	// ������ �̸��� ������ ���� �ʴ´�.
	// �ᱹ ��� bind�� �������� �ʴ´�.
	// ��Ƽüũ�� ������� ���濩�θ� Ȯ���ϰ� �ʿ��� ��� ���� �����ϴ� ���ҽ��� ó���ϴ� �Լ�����.
	// => SetRenderTargets, SetTexture�� �� ó���ϸ� �ʿ䰡 ������ �� ����.
	void Graphics::prepareDraw()
	{
		if (s_bRenderTargetViewsDirty)
		{
			// 1. DepthStencilView�� �����Ѵٸ� ���� ���ٸ� ����Ʈ ���̹���
			// 2. !���̾���, DepthSTencilView�� ReadOnly��� ReadOnly��...
			// 3. RenderTargets[0]�� �����, DepthStencil�� ���ų�, ������ ũ�Ⱑ ����۶� ���� �� ����Ʈ ����Ÿ��

			s_pDeviceContext->OMSetRenderTargets(4, &s_RenderTargetViews[0], s_pDefaultDepthStencilView);

			s_bRenderTargetViewsDirty = false;
		}

		if (s_bTextureDirty)
		{
			s_pDeviceContext->PSSetShaderResources(0, (uint32_t)eTextureUnit::Max_Num, &s_ShaderResourceViews[0]);
			s_pDeviceContext->PSSetSamplers(0, (uint32_t)eTextureUnit::Max_Num, &s_SamplerStates[0]);

			s_bTextureDirty = false;
		}
	}

	// beginFrame(), reiszeBackbuffer() ��� ȣ��
	void Graphics::resetViews()
	{
		for (uint32_t i = 0; i < 4; i++)
			SetRenderTargetView(i, nullptr);

		SetDepthStencilView(nullptr);

		RECT rt = { 0, 0, (LONG)s_ResolutionWidth, (LONG)s_ResolutionHeight };
		SetViewport(rt);
	}
}