#include "divepch.h"
#include "DvGraphics.h"
#include "DvWindow.h"
#include "Core/DvContext.h"
#include "Base/Base.h"
#include "IO/DvLog.h"


Dive::DvGraphics::DvGraphics(DvContext* pContext)
	: DvObject(pContext),
	m_pSwapChain(nullptr),
	m_pDevice(nullptr),
	m_pDeviceContext(nullptr)
{
	// Constructor_d3d11()
	// GraphicsImple 생성
	// => 별거 아니다.

	// 쉐이더 경로 및 파일 포멧 설정
	// 이외에도 기타 설정이 있다.
}

Dive::DvGraphics::~DvGraphics()
{
	DV_LOG_ENGINE_DEBUG("Graphics 소멸자 호출");
	Destroy();
}

bool Dive::DvGraphics::IsInitialized() const
{
	if (!m_pWindow || !m_pDevice || !m_pDeviceContext)
		return false;

	return true;
}

void Dive::DvGraphics::Destroy()
{
	// 각종 states

	DV_RELEASE(m_pDefaultDepthStencilView);
	DV_RELEASE(m_pDefaultDepthStencilTexture);
	DV_RELEASE(m_pDefaultRenderTargetView);

	DV_RELEASE(m_pSwapChain);
	DV_RELEASE(m_pDeviceContext);
	DV_RELEASE(m_pDevice);

	if (m_pWindow)
	{
		m_pWindow->Destroy();
		m_pWindow.release();
	}
}

bool Dive::DvGraphics::BeginFrame()
{
	if(!IsInitialized())
		return false;

	// 전체화면이면서 최소화 되어 있다면 false?
	// => 가능한 조합이 아닌 듯 한데...? 

	// reset render targets?
	// => 4개짜리 배열을 전부 nullptr로 초기화한다.
	// => GBuffer일 수도 있고, 아닐 수도 있다...

	DV_EVENT_FIRE(eDvEventType::BeginRender);

	return true;
}

void Dive::DvGraphics::EndFrame()
{
	if (!IsInitialized())
		return;

	DV_EVENT_FIRE(eDvEventType::EndRender);

	// vsync 여부를 멤버로 가져야 할 듯
	// orho는 screenParam으로 전부 가진다.
	m_pSwapChain->Present(1, 0);
}

void Dive::DvGraphics::SetTitle(const std::wstring& title)
{
	// 기본적으로 전달로 사용된다.
	m_WindowTitle = title;
	
	if (m_pWindow) 
		m_pWindow->ChangeTitle(title);
}

void Dive::DvGraphics::SetPosition(int x, int y)
{
	m_WindowPosition = DirectX::XMINT2(x, y);
	
	if (m_pWindow)
		m_pWindow->SetPosition(x, y);
}

bool Dive::DvGraphics::SetMode(int width, int height, bool bFullscreen, bool bBorderless, bool bResizable, bool bVSync,
	bool tripleBuffer, int multiSample, int refreshRate)
{
	ScreenModeParams params;
	params.bFullScreen = bFullscreen;
	params.bBorderless = bBorderless;
	params.bResizable = bResizable;
	params.bVSync = bVSync;

	WindowModeParams windowMode;
	windowMode.width = width;
	windowMode.height = height;
	windowMode.screenModeParams = params;

	const bool maximize = (!width || !height) && !params.bFullScreen && !params.bBorderless && params.bResizable;

	// 실제로는 함수들을 타고 들어가서
	// 윈도우 생성 후 D3d11Device를 초기화한다.
	// D3D11Device는 GraphicsImple이라는 추가 객체를 통해 관리한다. => 하지만 생성은 Graphics::CreateDevice_D3D11()에서 한다.
	// 즉, Renderer, Graphics, GraphicsImple을 구분해 놓아야 한다.
	// => 아무래도 Graphics와 GraphicsImple의 구분은 멀티 플랫폼때문에 API를 만들어 사용하기 때문인 듯 보인다.

	if (!m_pWindow)
	{
		unsigned int flags = 0;
		flags |= bFullscreen ? DV_WINDOW_FULLSCREEN : 0;
		flags |= bBorderless ? DV_WINDOW_BORDERLESS : 0;
		flags |= bResizable ? DV_WINDOW_RESIZABLE : 0;

		m_pWindow = std::make_unique<DvWindow>();
		if (!m_pWindow->Create(m_WindowTitle, m_WindowPosition.x, m_WindowPosition.y, width, height, flags))
		{
			m_pWindow.release();
			DV_LOG_ENGINE_ERROR("윈도우 생성에 실패하였습니다.");
			return false;
		}

		m_pWindow->Show();
	}

	// 현재 위치는 임시
	createDevice(width, height);
	updateSwapChain(width, height);
	
	return true;
}

bool Dive::DvGraphics::RunWindow()
{
	if(!m_pWindow)
		return false;

	return m_pWindow->Run();
}

// 임시
HWND Dive::DvGraphics::GetWindowHandle()
{
	if (!m_pWindow)
		return NULL;

	return m_pWindow->GetHandle();
}

bool Dive::DvGraphics::createDevice(int width, int height)
{
	if (!m_pDevice)
	{
		if (FAILED(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&m_pDevice,
			nullptr,
			&m_pDeviceContext
		)))
		{
			DV_RELEASE(m_pDevice);
			DV_RELEASE(m_pDeviceContext);
			DV_LOG_ENGINE_ERROR("D3D11 장치 생성에 실패하였습니다.");
			return false;
		}
	}

	// 멀티 샘플 레밸 체크?

	if (m_pSwapChain)
		DV_RELEASE(m_pSwapChain);

	IDXGIDevice* pDxgiDevice = nullptr;
	m_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
	IDXGIAdapter* pDxgiAdapter = nullptr;
	pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
	IDXGIFactory* pDxgiFactory = nullptr;
	pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

	// 동일 해상도에서 최적화된 리플래쉬 레이트 설정
	// 일단 건너뛴다.

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = (UINT)width;
	desc.BufferDesc.Height = (UINT)height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB???
	desc.BufferDesc.RefreshRate.Denominator = 1;			// 추후 수정(vsync에 따라 달라진다?)
	desc.BufferDesc.RefreshRate.Numerator = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;								// 멀티 샘플링 off
	desc.SampleDesc.Quality = 0;
	desc.Windowed = m_pWindow->IsFullScreen() ? FALSE : TRUE;	// orho는 TRUE 고정이다.
	desc.OutputWindow = m_pWindow->GetHandle();
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek에선 0이고 다른 값들 설정이 남아 있다...

	HRESULT hr = pDxgiFactory->CreateSwapChain(m_pDevice, &desc, &m_pSwapChain);
	
	DV_RELEASE(pDxgiFactory);
	DV_RELEASE(pDxgiAdapter);
	DV_RELEASE(pDxgiDevice);

	if(FAILED(hr))
	{
		DV_RELEASE(m_pSwapChain);
		DV_LOG_ENGINE_ERROR("D3D11 스왑체인 생성에 실패하였습니다.");
		return false;
	}

	return true;
}

// render target을 재생성하는 함수 같다.
// 기존에 전체화면 과정에서 구현했던 함수랑 비슷하다.
bool Dive::DvGraphics::updateSwapChain(int width, int height)
{
	// 빈 렌더타겟뷰를 Set
	ID3D11RenderTargetView* pNullView = nullptr;
	m_pDeviceContext->OMSetRenderTargets(1, &pNullView, nullptr);

	// 디폴트 리소스들을 전부 릴리즈
	DV_RELEASE(m_pDefaultRenderTargetView);
	DV_RELEASE(m_pDefaultDepthStencilTexture);
	DV_RELEASE(m_pDefaultDepthStencilView);

	// 현재 렌더타겟배열, 뎁스스텐실 뷰 nullptr

	// 리사이즈 버퍼
	m_pSwapChain->ResizeBuffers(1, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	// 백버퍼를 얻어와 디폴트 렌더타겟뷰 생성
	ID3D11Texture2D* pBackbufferTexture = nullptr;
	if (FAILED(m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
	{
		DV_RELEASE(pBackbufferTexture);
		DV_LOG_ENGINE_ERROR("후면 버퍼 텍스쳐 생성에 실패하였습니다.");
		return false;
	}

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackbufferTexture, nullptr, &m_pDefaultRenderTargetView)))
	{
		DV_RELEASE(m_pDefaultRenderTargetView);
		DV_LOG_ENGINE_ERROR("후면 버퍼 렌더타겟뷰 생성에 실패하였습니다.");
		return false;
	}
	DV_RELEASE(pBackbufferTexture);

	// 디폴트 뎁스스텐실 생성

	// 백버퍼 사이즈를 저장
	m_Width = width;
	m_Height = height;

	RECT rt;
	::GetWindowRect(m_pWindow->GetHandle(), &rt);
	DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

	::GetClientRect(m_pWindow->GetHandle(), &rt);
	DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

	return true;
}
