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
	// GraphicsImple ����
	// => ���� �ƴϴ�.

	// ���̴� ��� �� ���� ���� ����
	// �̿ܿ��� ��Ÿ ������ �ִ�.
}

Dive::DvGraphics::~DvGraphics()
{
	DV_LOG_ENGINE_DEBUG("Graphics �Ҹ��� ȣ��");
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
	// ���� states

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

	// ��üȭ���̸鼭 �ּ�ȭ �Ǿ� �ִٸ� false?
	// => ������ ������ �ƴ� �� �ѵ�...? 

	// reset render targets?
	// => 4��¥�� �迭�� ���� nullptr�� �ʱ�ȭ�Ѵ�.
	// => GBuffer�� ���� �ְ�, �ƴ� ���� �ִ�...

	DV_EVENT_FIRE(eDvEventType::BeginRender);

	return true;
}

void Dive::DvGraphics::EndFrame()
{
	if (!IsInitialized())
		return;

	DV_EVENT_FIRE(eDvEventType::EndRender);

	// vsync ���θ� ����� ������ �� ��
	// orho�� screenParam���� ���� ������.
	m_pSwapChain->Present(1, 0);
}

void Dive::DvGraphics::SetTitle(const std::wstring& title)
{
	// �⺻������ ���޷� ���ȴ�.
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

	// �����δ� �Լ����� Ÿ�� ����
	// ������ ���� �� D3d11Device�� �ʱ�ȭ�Ѵ�.
	// D3D11Device�� GraphicsImple�̶�� �߰� ��ü�� ���� �����Ѵ�. => ������ ������ Graphics::CreateDevice_D3D11()���� �Ѵ�.
	// ��, Renderer, Graphics, GraphicsImple�� ������ ���ƾ� �Ѵ�.
	// => �ƹ����� Graphics�� GraphicsImple�� ������ ��Ƽ �÷��������� API�� ����� ����ϱ� ������ �� ���δ�.

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
			DV_LOG_ENGINE_ERROR("������ ������ �����Ͽ����ϴ�.");
			return false;
		}

		m_pWindow->Show();
	}

	// ���� ��ġ�� �ӽ�
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

// �ӽ�
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
			DV_LOG_ENGINE_ERROR("D3D11 ��ġ ������ �����Ͽ����ϴ�.");
			return false;
		}
	}

	// ��Ƽ ���� ���� üũ?

	if (m_pSwapChain)
		DV_RELEASE(m_pSwapChain);

	IDXGIDevice* pDxgiDevice = nullptr;
	m_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
	IDXGIAdapter* pDxgiAdapter = nullptr;
	pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
	IDXGIFactory* pDxgiFactory = nullptr;
	pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

	// ���� �ػ󵵿��� ����ȭ�� ���÷��� ����Ʈ ����
	// �ϴ� �ǳʶڴ�.

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = (UINT)width;
	desc.BufferDesc.Height = (UINT)height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB???
	desc.BufferDesc.RefreshRate.Denominator = 1;			// ���� ����(vsync�� ���� �޶�����?)
	desc.BufferDesc.RefreshRate.Numerator = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc.Count = 1;								// ��Ƽ ���ø� off
	desc.SampleDesc.Quality = 0;
	desc.Windowed = m_pWindow->IsFullScreen() ? FALSE : TRUE;	// orho�� TRUE �����̴�.
	desc.OutputWindow = m_pWindow->GetHandle();
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek���� 0�̰� �ٸ� ���� ������ ���� �ִ�...

	HRESULT hr = pDxgiFactory->CreateSwapChain(m_pDevice, &desc, &m_pSwapChain);
	
	DV_RELEASE(pDxgiFactory);
	DV_RELEASE(pDxgiAdapter);
	DV_RELEASE(pDxgiDevice);

	if(FAILED(hr))
	{
		DV_RELEASE(m_pSwapChain);
		DV_LOG_ENGINE_ERROR("D3D11 ����ü�� ������ �����Ͽ����ϴ�.");
		return false;
	}

	return true;
}

// render target�� ������ϴ� �Լ� ����.
// ������ ��üȭ�� �������� �����ߴ� �Լ��� ����ϴ�.
bool Dive::DvGraphics::updateSwapChain(int width, int height)
{
	// �� ����Ÿ�ٺ並 Set
	ID3D11RenderTargetView* pNullView = nullptr;
	m_pDeviceContext->OMSetRenderTargets(1, &pNullView, nullptr);

	// ����Ʈ ���ҽ����� ���� ������
	DV_RELEASE(m_pDefaultRenderTargetView);
	DV_RELEASE(m_pDefaultDepthStencilTexture);
	DV_RELEASE(m_pDefaultDepthStencilView);

	// ���� ����Ÿ�ٹ迭, �������ٽ� �� nullptr

	// �������� ����
	m_pSwapChain->ResizeBuffers(1, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	// ����۸� ���� ����Ʈ ����Ÿ�ٺ� ����
	ID3D11Texture2D* pBackbufferTexture = nullptr;
	if (FAILED(m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
	{
		DV_RELEASE(pBackbufferTexture);
		DV_LOG_ENGINE_ERROR("�ĸ� ���� �ؽ��� ������ �����Ͽ����ϴ�.");
		return false;
	}

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackbufferTexture, nullptr, &m_pDefaultRenderTargetView)))
	{
		DV_RELEASE(m_pDefaultRenderTargetView);
		DV_LOG_ENGINE_ERROR("�ĸ� ���� ����Ÿ�ٺ� ������ �����Ͽ����ϴ�.");
		return false;
	}
	DV_RELEASE(pBackbufferTexture);

	// ����Ʈ �������ٽ� ����

	// ����� ����� ����
	m_Width = width;
	m_Height = height;

	RECT rt;
	::GetWindowRect(m_pWindow->GetHandle(), &rt);
	DV_LOG_ENGINE_DEBUG("Window Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

	::GetClientRect(m_pWindow->GetHandle(), &rt);
	DV_LOG_ENGINE_DEBUG("Client Size: {0:d} x {1:d}", rt.right - rt.left, rt.bottom - rt.top);

	return true;
}
