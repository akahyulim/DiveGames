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

		// 일단 Singleton으로 간다.
		// sub system 생성 & 초기화
		// job system으로 호출할 것인가...
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
			// 입력 버퍼만 수행
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
		// 뭔가 있다.
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