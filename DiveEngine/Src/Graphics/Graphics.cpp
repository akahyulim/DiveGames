#include "DivePch.h"
#include "Graphics.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	static HINSTANCE s_hInstance = 0;
	static HWND s_hWnd = 0;
	static std::wstring s_WindowTitle = L"DiveGames";
	static int s_Width = 1024;
	static int s_Height = 768;

	static IDXGISwapChain* s_pSwapChain = nullptr;
	static ID3D11Device* s_pDevice = nullptr;
	static ID3D11DeviceContext* s_pDeviceContext = nullptr;

	static ID3D11RenderTargetView* s_pDefaultRenderTargetView = nullptr;
	static ID3D11Texture2D* s_pDefaultDepthStencilTexture = nullptr;
	static ID3D11DepthStencilView* s_pDefaultDepthStencilView = nullptr;

	static ID3D11RenderTargetView* s_pRenderTargetView = nullptr;
	static ID3D11DepthStencilView* s_pDepthStencilView = nullptr;
	static bool s_bRenderTargetsDirty = false;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		}

		return Graphics::MessageHandler(hWnd, msg, wParam, lParam);
	}

	bool Graphics::Initialize()
	{
		if (!WindowCreate())
			return false;

		if (!CreateDeviceAndSwapChain())
			return false;

		if (!UpdateSwapChain())
			return false;

		DV_CORE_TRACE("Graphics �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void Graphics::Shutdown()
	{
		DV_RELEASE(s_pSwapChain);
		DV_RELEASE(s_pDeviceContext);
		DV_RELEASE(s_pDevice);

		::DestroyWindow(s_hWnd);
		::UnregisterClassW(L"Dive", s_hInstance);

		DV_CORE_TRACE("Graphics ���ῡ �����Ͽ����ϴ�.");
	}

	bool Graphics::WindowCreate()
	{
		s_hInstance = GetModuleHandle(nullptr);

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
		wc.lpszClassName = L"Dive";
		wc.cbSize = sizeof(WNDCLASSEX);
		RegisterClassEx(&wc);

		DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

		RECT rt = { 0, 0, s_Width, s_Height };
		::AdjustWindowRect(&rt, style, FALSE);

		int windowWidth = rt.right - rt.left;
		int windowHeight = rt.bottom - rt.top;

		int posX = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
		int posY = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;

		s_hWnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			L"Dive",
			s_WindowTitle.c_str(),
			style,
			posX, posY,
			windowWidth, windowHeight,
			NULL, NULL,
			s_hInstance,
			NULL);

		if (!s_hWnd)
		{
			DV_CORE_ERROR("Graphics::WindowCreate() - ������ ������ �����Ͽ����ϴ�.");
			return false;
		}

		GetClientRect(s_hWnd, &rt);
		DV_ASSERT(s_Width == rt.right - rt.left);
		DV_ASSERT(s_Height == rt.bottom - rt.top);

		::ShowWindow(s_hWnd, SW_SHOW);
		::SetFocus(s_hWnd);

		DV_CORE_TRACE("{0:d} x {1:d} ������ ������ �����Ͽ����ϴ�.", s_Width, s_Height);

		return true;
	}

	bool Graphics::RunWindow()
	{
		if (!IsInitialized())
			return false;

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
		if (IsInitialized())
		{
			//switch (msg)
			{
				//default:
				//	return 0;
			}
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	bool Graphics::IsInitialized()
	{
		return (s_hWnd && s_pDevice && s_pDeviceContext);
	}

	HINSTANCE Graphics::GetWindowInstance()
	{
		return s_hInstance;
	}

	HWND Graphics::GetWindowHandle()
	{
		return s_hWnd;
	}
	
	std::wstring Graphics::GetWindowTitle()
	{
		return s_WindowTitle;
	}

	int Graphics::GetWidth()
	{
		return s_Width;
	}
	
	int Graphics::GetHeight()
	{
		return s_Height;
	}

	void Graphics::SetWindowTitle(const std::wstring& title)
	{
		if (s_hWnd)
			::SetWindowText(s_hWnd, title.c_str());

		s_WindowTitle = title;
	}

	/*
	���� â���� �����Ǿ� �ִ�.
	*/
	bool Graphics::CreateDeviceAndSwapChain()
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		if (FAILED(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			&featureLevel,//nullptr,
			1, //0,
			D3D11_SDK_VERSION,
			&s_pDevice,
			nullptr,
			&s_pDeviceContext)))
		{
			DV_CORE_ERROR("Graphics::CreateDeviceAndSwapChain - D3D11 ��ġ ������ �����Ͽ����ϴ�.");
			return false;
		}

		IDXGIDevice* pDxgiDevice = nullptr;
		s_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter = nullptr;
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory = nullptr;
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 1;
		desc.BufferDesc.Width = s_Width;
		desc.BufferDesc.Height = s_Height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB???
		desc.BufferDesc.RefreshRate.Denominator = 1;			// ���� ����(vsync�� ���� �޶�����?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// ��Ƽ ���ø� off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = true;
		desc.OutputWindow = s_hWnd;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek���� 0�̰� �ٸ� ���� ������ ���� �ִ�...

		if (FAILED(pDxgiFactory->CreateSwapChain(s_pDevice, &desc, &s_pSwapChain)))
		{
			DV_CORE_ERROR("Graphics::CreateDeviceAndSwapChain - D3D11 ����ü�� ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}

	bool Graphics::UpdateSwapChain(uint32_t width, uint32_t height)
	{
		if (!IsInitialized())
			return false;

		s_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		DV_RELEASE(s_pDefaultRenderTargetView);
		DV_RELEASE(s_pDefaultDepthStencilTexture);
		DV_RELEASE(s_pDefaultDepthStencilView);

		s_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		// ����۸� ���� ����Ʈ ����Ÿ�ٺ� ����
		ID3D11Texture2D* pBackbufferTexture = nullptr;
		if (FAILED(s_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_CORE_ERROR("Graphics::UpdateSwapChain - �ĸ� ���� �ؽ��� ������ �����Ͽ����ϴ�.");
			return false;
		}

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

		if (FAILED(s_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), nullptr, &s_pDefaultRenderTargetView)))
		{
			DV_CORE_ERROR("Graphics::UpdateSwapChain - �ĸ� ���� ����Ÿ�ٺ� ������ �����Ͽ����ϴ�.");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		{
			// ����Ʈ �������ٽ� ����
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = curWidth;
			desc.Height = curHeight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.SampleDesc.Count = 1;// static_cast<UINT>(screenParams_.multiSample_);
			desc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(desc.Format, screenParams_.multiSample_);
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			if (FAILED(s_pDevice->CreateTexture2D(&desc, nullptr, &s_pDefaultDepthStencilTexture)))
			{
				DV_RELEASE(s_pDefaultDepthStencilTexture);
				DV_CORE_ERROR("Graphics::UpdateSwapChain - ����� ���� ���ٽ� �ؽ��� ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;

			if (FAILED(s_pDevice->CreateDepthStencilView(
				static_cast<ID3D11Resource*>(s_pDefaultDepthStencilTexture),
				&desc,//nullptr,		// urho�� nullptr�� �����ߴ�. ���ó� sampler ������ �ذ���� �ʾҴ�.
				&s_pDefaultDepthStencilView)))
			{
				DV_RELEASE(s_pDefaultDepthStencilView);
				DV_CORE_ERROR("Graphics::UpdateSwapChain - ����� ���� ���ٽ� �� ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		s_Width = static_cast<int32_t>(curWidth);
		s_Height = static_cast<int32_t>(curHeight);

		return true;
	}

	bool Graphics::BeginFrame()
	{
		if (!IsInitialized())
			return false;

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		s_pSwapChain->Present(1, 0);
	}

	void Graphics::ClearRenderTargets(uint32_t target, const DirectX::XMFLOAT4& color, float depth, uint8_t stencil)
	{
		prepareDraw();

		if(target & eClearTarget::Color)
		{
			float clearColor[4] = { color.x, color.y, color.z, color.w };
			s_pDeviceContext->ClearRenderTargetView(s_pRenderTargetView, clearColor);
		}

		if((target & eClearTarget::Depth) || (target & eClearTarget::Stencil))
		{
			uint32_t flags = 0;
			flags |= (target & eClearTarget::Depth) ? D3D11_CLEAR_DEPTH : 0;
			flags |= (target & eClearTarget::Stencil) ? D3D11_CLEAR_STENCIL : 0;

			s_pDeviceContext->ClearDepthStencilView(s_pDepthStencilView, flags, depth, stencil);
		}
	}

	void Graphics::SetRenderTargetViews(uint32_t index, ID3D11RenderTargetView* pViews)
	{
		if (s_pRenderTargetView == pViews)
			return;

		s_pRenderTargetView = pViews;
		s_bRenderTargetsDirty = true;
	}

	void Graphics::SetDepthStencilView(ID3D11DepthStencilView* pView)
	{
		if (s_pDepthStencilView == pView)
			return;

		s_pDepthStencilView = pView;
		s_bRenderTargetsDirty = true;
	}

	ID3D11Device* Graphics::GetDevice()
	{
		return s_pDevice;
	}
	
	ID3D11DeviceContext* Graphics::GetDeviceContext()
	{
		return s_pDeviceContext;
	}

	ID3D11RenderTargetView* Graphics::GetDefaultRenderTargetView()
	{
		return s_pDefaultRenderTargetView;
	}

	ID3D11DepthStencilView* Graphics::GetDefaultDepthStencilView()
	{
		return s_pDefaultDepthStencilView;
	}

	void Graphics::prepareDraw()
	{
		if (s_bRenderTargetsDirty)
		{
			s_pDeviceContext->OMSetRenderTargets(1, &s_pRenderTargetView, s_pDepthStencilView);

			s_bRenderTargetsDirty = false;
		}
	}
}