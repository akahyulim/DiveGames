#include "divepch.h"
#include "DvGraphics.h"
#include "DvWindow.h"
#include "Core/DvContext.h"
#include "IO/DvLog.h"


Dive::DvGraphics::DvGraphics(DvContext* pContext)
	: DvObject(pContext)
{
	// Constructor_d3d11()
	// GraphicsImple 생성
	// 쉐이더 경로 및 파일 포멧 설정
	// 이외에도 기타 설정이 있다.
}

Dive::DvGraphics::~DvGraphics()
{
	DV_LOG_ENGINE_DEBUG("Graphics 소멸자 호출");
}

bool Dive::DvGraphics::IsInitialized() const
{
	// window, device, context 유무
	if (!m_pWindow)
		return false;

	return true;
}

bool Dive::DvGraphics::BeginFrame()
{
	if(!IsInitialized())
		return false;

	// reset render targets?

	return true;
}

void Dive::DvGraphics::EndFrame()
{
	if (!IsInitialized())
		return;

	// swapchain present
}

void Dive::DvGraphics::SetWindowTitle(const std::wstring& title)
{
	// 기본적으로 전달로 사용된다.
	m_WindowTitle = title;
	
	if (m_pWindow)
	{
		// 이미 생성되었다면 교체한다.
	}
}

void Dive::DvGraphics::SetWindowPosition(int x, int y)
{
	m_WindowPosition = DirectX::XMINT2(x, y);
	
	if (m_pWindow)
	{
	}
}

bool Dive::DvGraphics::SetMode(int width, int height, bool bFullScreen, bool bBorderless, bool bResizable, bool bVSync)
{
	ScreenModeParams params;
	params.bFullScreen = bFullScreen;
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
		m_pWindow = std::make_unique<DvWindow>();
		if (!m_pWindow->DvCreateWindow(m_WindowTitle, m_WindowPosition.x, m_WindowPosition.y, width, height, 0))
		{
			m_pWindow.release();
			DV_LOG_ENGINE_ERROR("윈도우 생성에 실패하였습니다.");
			return false;
		}
	}

	return true;
}

bool Dive::DvGraphics::RunWindow()
{
	//if (!IsInitialized())
	if(!m_pWindow)
		return false;

	return m_pWindow->Run();
}
