#include "divepch.h"
#include "DvGraphics.h"
#include "DvWindow.h"
#include "Core/DvContext.h"
#include "IO/DvLog.h"


Dive::DvGraphics::DvGraphics(DvContext* pContext)
	: DvObject(pContext)
{
	// Constructor_d3d11()
	// GraphicsImple ����
	// ���̴� ��� �� ���� ���� ����
	// �̿ܿ��� ��Ÿ ������ �ִ�.
}

Dive::DvGraphics::~DvGraphics()
{
	DV_LOG_ENGINE_DEBUG("Graphics �Ҹ��� ȣ��");
}

bool Dive::DvGraphics::IsInitialized() const
{
	// window, device, context ����
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
	// �⺻������ ���޷� ���ȴ�.
	m_WindowTitle = title;
	
	if (m_pWindow)
	{
		// �̹� �����Ǿ��ٸ� ��ü�Ѵ�.
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

	// �����δ� �Լ����� Ÿ�� ����
	// ������ ���� �� D3d11Device�� �ʱ�ȭ�Ѵ�.
	// D3D11Device�� GraphicsImple�̶�� �߰� ��ü�� ���� �����Ѵ�. => ������ ������ Graphics::CreateDevice_D3D11()���� �Ѵ�.
	// ��, Renderer, Graphics, GraphicsImple�� ������ ���ƾ� �Ѵ�.
	// => �ƹ����� Graphics�� GraphicsImple�� ������ ��Ƽ �÷��������� API�� ����� ����ϱ� ������ �� ���δ�.

	if (!m_pWindow)
	{
		m_pWindow = std::make_unique<DvWindow>();
		if (!m_pWindow->DvCreateWindow(m_WindowTitle, m_WindowPosition.x, m_WindowPosition.y, width, height, 0))
		{
			m_pWindow.release();
			DV_LOG_ENGINE_ERROR("������ ������ �����Ͽ����ϴ�.");
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
