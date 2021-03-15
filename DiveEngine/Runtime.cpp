#include "Runtime.h"
#include "Log.h"
#include "Timer.h"
#include "ThreadPool.h"
#include "RenderPath.h"
#include "Renderer.h"
#include <memory>

using namespace std;

namespace Dive
{
	Runtime::Runtime()
	{
		Log::Initialize();
	}

	void Runtime::Initialize()
	{
		if (m_bInitialized)
		{
			return;
		}
		m_bInitialized = true;
		
		CORE_TRACE("Dive Engine 초기화를 시작합니다...");

		ThreadPool::GetInstance().Initialize();

		// 현재 ThreadPool 결과가 이상하다.

		TimeManager::GetInstance().Initialize();
		Renderer::GetInstance().Initialize();
	}

	void Runtime::Run()
	{
		if (!m_bInitialized)
		{
			Initialize();
			
			if (!m_bInitialized)
			{
				CORE_ERROR("Rumtime 초기화에 실패하였습니다. 프로그램을 종료합니다.");
				PostQuitMessage(0);
			}
		}

		TimeManager::GetInstance().Update();
		CORE_TRACE("FrameCount: {0:n}, ElapsedTime: {1:n}", TimeManager::GetInstance().GetFrameCount(), TimeManager::GetInstance().GetRealTimeSinceStartUpSec());

		//m_deltaTime = static_cast<float>(max(0.0, m_timer.GetElapsedTime() / 1000.0f));
		//m_timer.Record();

		if (m_bWindowActive)
		{
			// 일단 타이머가 초단위로 값을 가진다... 0.2sec
			// 따라서 elapsed time을 누적한 후 값을 비교하여 실행 여부를 판단해야 한다.
			// 그게 아니라면 TimeManager가 이 함수를 호출하는게 된다. 물론 이벤트라면 가능할 거 같긴한데...
			// 처음에 생각했던 것 처럼 Runtime에서 FixedTime을 관리하는 것도 생각해볼 수 있다.
			// 하지만 그 경우에도 결국엔 FixedTime을 FixedUpdate()에 전달해주는 것이 자연스럽다.
			// 이를 Update()와 연관시켜보면 결국 매개변수는 없애고 필요할 때 TimeManager 객체를 통해 
			// FixedTime과 ElapsedTime을 획득하는 것이 가장 깔끔하다. 유니티에서도 그렇게 구현되어 있다.
			FixedUpdate();
			
			Update(m_deltaTime);
			// 입력 버퍼
			Render();
			
		}
		else
		{
			// 입력 버퍼만 수행
		}

		// Present 실행
		auto device = Renderer::GetInstance().GetDevice();
		auto cmd = device->BeginCommandList();
		device->PresentBegin(cmd);
		Compose(cmd);
		device->PresentEnd();
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

	void Runtime::Compose(CommandList cmd)
	{
		if (m_activePath)
		{
			m_activePath->Compose(cmd);
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
		auto& renderer = Renderer::GetInstance();
		renderer.SetDevice(make_shared<GraphicsDevice>(windowHandle, fullScreen));

		auto device = renderer.GetDevice();
		CORE_TRACE("Resolution: {0:d} x {1:d}", device->GetResolutionWidth(), device->GetResolutionHeight());

		RECT rt;
		GetClientRect(windowHandle, &rt);
		auto width = static_cast<unsigned int>(rt.right - rt.left);
		auto height = static_cast<unsigned int>(rt.bottom - rt.top);
		CORE_TRACE("Changed ClientRect: {0:d} x {1:d}", width, height);

		device->SetResolution(800, 600);
		CORE_TRACE("Changed Resolution: {0:d} x {1:d}", device->GetResolutionWidth(), device->GetResolutionHeight());
	}
}