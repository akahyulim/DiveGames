#include "divepch.h"
#include "Engine.h"
#include "Log.h"

namespace Dive
{
	void Engine::Initialize(const WindowData* pData)
	{
		Log::Initialize();
		m_Time.Initialize();

		Renderer::Initialize(pData);
	}

	void Engine::Shutdown()
	{
		Renderer::Shutdown();
	}

	void Engine::Run()
	{
		m_Time.Tick();
	}

	void Engine::SetWindowData(const WindowData& data)
	{
		m_WindowData = data;

		// ���� �̺�Ʈ�� �����Ѵ�.
		// �׷��� Hazel�� Spartan�� Event System�� �ٸ���.
		// ����Ƶ� �޽������� �̺�Ʈ�� ������ �Ѵ�.

		// �ӽ� ó��
		auto& graphicsDevice = Renderer::GetGraphicsDevice();
		// �̷��� �޽������� Ȯ���ϴ� �� ����
		// �ܼ� ũ�� �񱳰� �� �¾� �����µ�
		// ���� �����غ��� �����δ�...
		if (data.msg == WM_SIZE && data.wParam != SIZE_MINIMIZED)
		{
			// �׸��� �Լ��� ũ�⸦ �����ϴ� ���� �´�.
			graphicsDevice.CleanupMainRenderTargetView();
			graphicsDevice.GetSwapChain()->ResizeBuffers(0, data.Width, data.Height, DXGI_FORMAT_UNKNOWN, 0);
			graphicsDevice.CreateMainRenderTargetView();
		}
	}
}