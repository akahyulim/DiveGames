#include "DivePch.h"
#include "Graphics.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Texture2D.h"
#include "RenderTexture.h"
#include "ShaderVariation.h"
#include "ConstantBuffer.h"
#include "InputLayout.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	static HINSTANCE s_hInstance = 0;
	static HWND s_hWnd = 0;
	static std::wstring s_WindowTitle = L"DiveGames";
	static int s_Width = 800;
	static int s_Height = 600;

	static IDXGISwapChain* s_pSwapChain = nullptr;
	static ID3D11Device* s_pDevice = nullptr;
	static ID3D11DeviceContext* s_pDeviceContext = nullptr;

	static ID3D11RenderTargetView* s_pDefaultRenderTargetView = nullptr;
	static ID3D11Texture2D* s_pDefaultDepthStencilTexture = nullptr;
	static ID3D11DepthStencilView* s_pDefaultDepthStencilView = nullptr;

	static ID3D11RenderTargetView* s_pRenderTargetViews[MAX_RENDERTARGETS] = { nullptr, };
	static ID3D11DepthStencilView* s_pDepthStencilView = nullptr;
	static bool s_bRenderTargetsDirty = false;

	static Texture* s_pTextures[static_cast<uint32_t>(eTextureUnit::Max_Num)] = { nullptr, };
	static ID3D11ShaderResourceView* s_pShaderResourceViews[static_cast<uint32_t>(eTextureUnit::Max_Num)] = { nullptr, };
	static ID3D11SamplerState* s_pSamplerStates[static_cast<uint32_t>(eTextureUnit::Max_Num)] = { nullptr, };
	static bool s_bTextureDirty = false;
	static uint32_t s_TextureDirtyStart = 0xffffffff;
	static uint32_t s_TextureDirtyEnd = 0xffffffff;

	static ID3D11DepthStencilState* s_pDepthStencilState = nullptr;
	static uint32_t s_StencilRef = 0;
	static bool s_bDepthStencilStateDirty = false;

	static D3D11_PRIMITIVE_TOPOLOGY s_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	static VertexBuffer* s_pVertexBuffer = nullptr;
	static IndexBuffer* s_pIndexBuffer = nullptr;

	static InputLayout* s_pInputLayout = nullptr;
	static ShaderVariation* s_pVertexShaderVariation = nullptr;
	static ShaderVariation* s_pPixelShaderVariation = nullptr;
	static ShaderVariation* s_pComputeShaderVariation = nullptr;

	static ID3D11Buffer* s_pVertexShaderConstantBuffers[2] = { nullptr, };
	static ID3D11Buffer* s_pPixelShaderConstantBuffers[3] = { nullptr, };


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

		DV_CORE_TRACE("Graphics 초기화에 성공하였습니다.");

		return true;
	}

	void Graphics::Shutdown()
	{
		DV_RELEASE(s_pSwapChain);
		DV_RELEASE(s_pDeviceContext);
		DV_RELEASE(s_pDevice);

		::DestroyWindow(s_hWnd);
		::UnregisterClassW(L"Dive", s_hInstance);

		DV_CORE_TRACE("Graphics 종료에 성공하였습니다.");
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
			DV_CORE_ERROR("Graphics::WindowCreate() - 윈도우 생성에 실패하였습니다.");
			return false;
		}

		GetClientRect(s_hWnd, &rt);
		DV_ASSERT(s_Width == rt.right - rt.left);
		DV_ASSERT(s_Height == rt.bottom - rt.top);

		::ShowWindow(s_hWnd, SW_SHOW);
		::SetFocus(s_hWnd);

		DV_CORE_TRACE("{0:d} x {1:d} 윈도우 생성에 성공하였습니다.", s_Width, s_Height);

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

	DirectX::XMINT2 Graphics::GetSize()
	{
		return DirectX::XMINT2(s_Width, s_Height);
	}

	void Graphics::SetWindowTitle(const std::wstring& title)
	{
		if (s_hWnd)
			::SetWindowText(s_hWnd, title.c_str());

		s_WindowTitle = title;
	}

	/*
	현재 창모드로 강제되어 있다.
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
			DV_CORE_ERROR("Graphics::CreateDeviceAndSwapChain - D3D11 장치 생성에 실패하였습니다.");
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
		desc.BufferDesc.RefreshRate.Denominator = 1;			// 추후 수정(vsync에 따라 달라진다?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// 멀티 샘플링 off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = true;
		desc.OutputWindow = s_hWnd;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek에선 0이고 다른 값들 설정이 남아 있다...

		if (FAILED(pDxgiFactory->CreateSwapChain(s_pDevice, &desc, &s_pSwapChain)))
		{
			DV_CORE_ERROR("Graphics::CreateDeviceAndSwapChain - D3D11 스왑체인 생성에 실패하였습니다.");
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

		// 백버퍼를 얻어와 디폴트 렌더타겟뷰 생성
		ID3D11Texture2D* pBackbufferTexture = nullptr;
		if (FAILED(s_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_CORE_ERROR("Graphics::UpdateSwapChain - 후면 버퍼 텍스쳐 생성에 실패하였습니다.");
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
			DV_CORE_ERROR("Graphics::UpdateSwapChain - 후면 버퍼 렌더타겟뷰 생성에 실패하였습니다.");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		{
			// 디폴트 뎁스스텐실 생성
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
				DV_CORE_ERROR("Graphics::UpdateSwapChain - 백버퍼 깊이 스텐실 텍스쳐 생성에 실패하였습니다.");
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
				&desc,//nullptr,		// urho가 nullptr을 전달했다. 역시나 sampler 문제는 해결되지 않았다.
				&s_pDefaultDepthStencilView)))
			{
				DV_RELEASE(s_pDefaultDepthStencilView);
				DV_CORE_ERROR("Graphics::UpdateSwapChain - 백버퍼 깊이 스텐실 뷰 생성에 실패하였습니다.");
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

	void Graphics::ClearViews(uint32_t target, const DirectX::XMFLOAT4& color, float depth, uint8_t stencil)
	{
		DV_ASSERT(IsInitialized());

		prepareDraw();

		if(target & eClearTarget::Color)
		{
			float clearColor[4] = { color.x, color.y, color.z, color.w };
			for (uint32_t i = 0; i < MAX_RENDERTARGETS; ++i)
				s_pDeviceContext->ClearRenderTargetView(s_pRenderTargetViews[i], clearColor);
		}

		if((target & eClearTarget::Depth) || (target & eClearTarget::Stencil))
		{
			uint32_t flags = 0;
			flags |= (target & eClearTarget::Depth) ? D3D11_CLEAR_DEPTH : 0;
			flags |= (target & eClearTarget::Stencil) ? D3D11_CLEAR_STENCIL : 0;

			s_pDeviceContext->ClearDepthStencilView(s_pDepthStencilView, flags, depth, stencil);
		}
	}

	void Graphics::Draw(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t vertexCount, uint32_t vertexStart)
	{
		DV_ASSERT(IsInitialized());

		if (!vertexCount)
			return;

		prepareDraw();

		if (s_PrimitiveTopology != type)
		{
			s_pDeviceContext->IASetPrimitiveTopology(type);
			s_PrimitiveTopology = type;
		}

		s_pDeviceContext->Draw(vertexCount, vertexStart);
	}

	void Graphics::DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t indexCount, uint32_t indexStart, uint32_t vertexStart)
	{
		DV_ASSERT(IsInitialized());

		prepareDraw();

		if (s_PrimitiveTopology != type)
		{
			s_pDeviceContext->IASetPrimitiveTopology(type);
			s_PrimitiveTopology = type;
		}

		s_pDeviceContext->DrawIndexed(indexCount, indexStart, vertexStart);
	}

	VertexBuffer* Graphics::GetVertexBuffer()
	{
		return s_pVertexBuffer;
	}

	void Graphics::SetVertexBuffer(VertexBuffer* pVertexBuffer, uint32_t offset)
	{
		DV_ASSERT(IsInitialized());

		if (pVertexBuffer != s_pVertexBuffer)
		{
			if (pVertexBuffer)
			{
				ID3D11Buffer* pBuffer = pVertexBuffer->GetBuffer();
				UINT stride = pVertexBuffer->GetStride();
				UINT offsets[] = { offset };

				s_pDeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &stride, offsets);
			}
			else
			{
				s_pDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
			}

			s_pVertexBuffer = pVertexBuffer;
		}
	}

	IndexBuffer* Graphics::GetIndexBuffer()
	{
		return s_pIndexBuffer;
	}

	void Graphics::SetIndexBuffer(IndexBuffer* pIndexBuffer, uint32_t offset)
	{
		DV_ASSERT(IsInitialized());

		if (pIndexBuffer != s_pIndexBuffer)
		{
			if (pIndexBuffer)
			{
				ID3D11Buffer* pOldBuffer = nullptr;
				DXGI_FORMAT oldFormat = pIndexBuffer->GetFormat();
				
				s_pDeviceContext->IASetIndexBuffer(pIndexBuffer->GetBuffer(), pIndexBuffer->GetFormat(), offset);
			}
			else
			{
				s_pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
			}

			s_pIndexBuffer = pIndexBuffer;
		}
	}

	InputLayout* Graphics::GetInputLayout()
	{
		return s_pInputLayout;
	}

	void Graphics::SetShaderVariation(eShaderType type, ShaderVariation* pShaderVariation)
	{
		DV_ASSERT(IsInitialized());

		if (type == eShaderType::VertexShader)
		{
			if (s_pVertexShaderVariation != pShaderVariation)
			{
				s_pDeviceContext->VSSetShader(pShaderVariation ?
					static_cast<ID3D11VertexShader*>(pShaderVariation->GetShaderResource()) : nullptr, nullptr, 0);
				s_pVertexShaderVariation = pShaderVariation;
					
				if (pShaderVariation && s_pInputLayout != pShaderVariation->GetInputLayout())
				{
					s_pDeviceContext->IASetInputLayout(pShaderVariation->GetInputLayout() ? 
						pShaderVariation->GetInputLayout()->GetInputLayout() : nullptr);
					s_pInputLayout = pShaderVariation->GetInputLayout();
				}
			}
		}
		else if (type == eShaderType::PixelShader)
		{
			if (s_pPixelShaderVariation != pShaderVariation)
			{
				s_pDeviceContext->PSSetShader(pShaderVariation ?
					static_cast<ID3D11PixelShader*>(pShaderVariation->GetShaderResource()) : nullptr, nullptr, 0);
				s_pPixelShaderVariation = pShaderVariation;
			}
		}
		else
		{
			if (s_pComputeShaderVariation != pShaderVariation)
			{
				s_pDeviceContext->CSSetShader(pShaderVariation ?
					static_cast<ID3D11ComputeShader*>(pShaderVariation->GetShaderResource()) : nullptr, nullptr, 0);
				s_pComputeShaderVariation = pShaderVariation;
			}
		}
	}

	// urho의 경우 SetShaders()에서 해당 Shader가 사용하는 Buffer를 직접 구성한다.
	// 추후 참조하자.
	void Graphics::SetConstantBuffer(uint8_t index, eShaderType type, ConstantBuffer* pBuffer)
	{
		auto pConstantBuffer = pBuffer->GetBuffer();

		if (type == eShaderType::VertexShader)
		{
			s_pVertexShaderConstantBuffers[index] = pConstantBuffer;
		}
		if (type == eShaderType::PixelShader)
		{
			s_pPixelShaderConstantBuffers[index] = pConstantBuffer;
		}
		if (type == eShaderType::ComputeShader)
		{
		}
	}

	void Graphics::SetRenderTargetView(uint32_t index, ID3D11RenderTargetView* pView)
	{
		DV_ASSERT(index < MAX_RENDERTARGETS);

		if (s_pRenderTargetViews[index] == pView)
			return;

		s_pRenderTargetViews[index] = pView;
		s_bRenderTargetsDirty = true;
	}

	void Graphics::SetRenderTargetViews(uint32_t start, uint32_t count, ID3D11RenderTargetView** ppViews)
	{
		DV_ASSERT(start + count < MAX_RENDERTARGETS);

		for (uint32_t i = 0; i < count; ++i)
		{
			if (s_pRenderTargetViews[start + i] != ppViews[i])
			{
				s_pRenderTargetViews[start + i] = ppViews[i];
				s_bRenderTargetsDirty = true;
			}
		}
	}

	void Graphics::SetDepthStencilView(ID3D11DepthStencilView* pView)
	{
		if (s_pDepthStencilView == pView)
			return;

		s_pDepthStencilView = pView;
		s_bRenderTargetsDirty = true;
	}

	void Graphics::SetDepthStencilState(ID3D11DepthStencilState* pDepthStencilState, uint32_t stencilRef)
	{
		if (s_pDepthStencilState != pDepthStencilState)
		{
			s_pDepthStencilState = pDepthStencilState;
			s_StencilRef = stencilRef;
			s_bDepthStencilStateDirty = true;
		}
	}

	Texture* Graphics::GetTexture(eTextureUnit unit)
	{
		return s_pTextures[static_cast<uint32_t>(unit)];
	}

	void Graphics::SetTexture(eTextureUnit unit, Texture* pTexture)
	{
		DV_ASSERT(unit != eTextureUnit::Max_Num);

		uint32_t index = static_cast<uint32_t>(unit);

		if (pTexture)
		{
			if (pTexture->IsMipLevelsDirty())
				pTexture->UpdateMipLevels();

			if (pTexture->IsSamplerStateDirty())
			{
				pTexture->UpdateSamplerState();
				s_pTextures[index] = nullptr;
			}
		}

		if (s_TextureDirtyStart == 0xffffffff)
			s_TextureDirtyStart = s_TextureDirtyEnd = index;
		else
		{
			if (index < s_TextureDirtyStart)
				s_TextureDirtyStart = index;
			else if (index > s_TextureDirtyEnd)
				s_TextureDirtyEnd = index;
		}

		if (pTexture != s_pTextures[index])
		{
			s_pTextures[index] = pTexture;
			s_pShaderResourceViews[index] = pTexture ? pTexture->GetShaderResourceView() : nullptr;
			s_pSamplerStates[index] = pTexture ? pTexture->GetSamplerState() : nullptr;
			s_bTextureDirty = true;
		}
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

	// 단순 캡슐함수 이상의 역할을 하지 못하고 있다...
	// => 각종 리소스들은 배열로 관리한 후 한 번에 보내는 편이 나아보인다. 그러므로 쓸모 있다.
	void Graphics::prepareDraw()
	{
		DV_ASSERT(IsInitialized());

		if (s_bRenderTargetsDirty)
		{
			s_pDeviceContext->OMSetRenderTargets(MAX_RENDERTARGETS, &s_pRenderTargetViews[0], s_pDepthStencilView);

			s_bRenderTargetsDirty = false;
		}

		if (s_bDepthStencilStateDirty)
		{
			s_pDeviceContext->OMSetDepthStencilState(s_pDepthStencilState, s_StencilRef);
			s_bDepthStencilStateDirty = false;
		}

		if (s_bTextureDirty)
		{
			s_pDeviceContext->PSSetShaderResources(
				s_TextureDirtyStart, 
				s_TextureDirtyEnd - s_TextureDirtyStart + 1, 
				&s_pShaderResourceViews[s_TextureDirtyStart]);
			s_pDeviceContext->PSSetSamplers(
				s_TextureDirtyStart,
				s_TextureDirtyEnd - s_TextureDirtyStart + 1, 
				&s_pSamplerStates[s_TextureDirtyStart]);

			s_TextureDirtyStart = s_TextureDirtyEnd = 0xffffffff;
			s_bTextureDirty = false;
		}

		{
			s_pDeviceContext->VSSetConstantBuffers(0, 2, &s_pVertexShaderConstantBuffers[0]);
			s_pDeviceContext->PSSetConstantBuffers(0, 3, &s_pPixelShaderConstantBuffers[0]);
		}
	}
}