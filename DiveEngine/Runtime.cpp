#include "Runtime.h"
#include "RenderPath.h"
#include "GraphicsDevice.h"
#include "Renderer.h"
#include <memory>

using namespace std;

namespace Dive
{
	void Runtime::Initialize()
	{
		if (m_bInitialized)
		{
			return;
		}
		m_bInitialized = true;

		// �ϴ� Singleton���� ����.
		// sub system ���� & �ʱ�ȭ
		// job system���� ȣ���� ���ΰ�...
		Renderer::GetInstance().Initialize();
	}

	void Runtime::Run()
	{
		if (!m_bInitialized)
		{
			Initialize();
		}

		m_deltaTime = static_cast<float>(max(0.0, m_timer.GetElapsedTime() / 1000.0f));
		m_timer.Record();

		if (m_bWindowActive)
		{
			Update(m_deltaTime);
			Render();
		}
		else
		{
			// �Է� ���۸� ����
		}
	}
	
	void Runtime::Update(float deltaTime)
	{
		if (m_activePath)
		{
			m_activePath->PreUpdate();
		}

		// lua

		if (m_activePath)
		{
			m_activePath->Update(deltaTime);
			m_activePath->PostUpdate();
		}
	}
	
	void Runtime::FixedUpdate()
	{
		// lua

		if (m_activePath)
		{
			m_activePath->FixedUpdate();
		}
	}
	
	void Runtime::Render()
	{
		// lua

		if (m_activePath)
		{
			m_activePath->Render();
		}
	}

	void Runtime::SetActivePath(RenderPath* path)
	{
		// ���� �ִ�.
		{
			if (m_activePath)
			{
				m_activePath->Stop();
			}

			if (path)
			{
				path->Start();
			}
			m_activePath = path;
		}
	}
	
	void Runtime::SetWindow(HWND windowHandle, bool fullScreen)
	{
		auto renderer = Renderer::GetInstance();
		renderer.SetDevice(make_shared<GraphicsDevice>(windowHandle, fullScreen));
	}
}