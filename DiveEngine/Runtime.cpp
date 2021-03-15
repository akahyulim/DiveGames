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
		
		CORE_TRACE("Dive Engine �ʱ�ȭ�� �����մϴ�...");

		ThreadPool::GetInstance().Initialize();

		// ���� ThreadPool ����� �̻��ϴ�.

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
				CORE_ERROR("Rumtime �ʱ�ȭ�� �����Ͽ����ϴ�. ���α׷��� �����մϴ�.");
				PostQuitMessage(0);
			}
		}

		TimeManager::GetInstance().Update();
		CORE_TRACE("FrameCount: {0:n}, ElapsedTime: {1:n}", TimeManager::GetInstance().GetFrameCount(), TimeManager::GetInstance().GetRealTimeSinceStartUpSec());

		//m_deltaTime = static_cast<float>(max(0.0, m_timer.GetElapsedTime() / 1000.0f));
		//m_timer.Record();

		if (m_bWindowActive)
		{
			// �ϴ� Ÿ�̸Ӱ� �ʴ����� ���� ������... 0.2sec
			// ���� elapsed time�� ������ �� ���� ���Ͽ� ���� ���θ� �Ǵ��ؾ� �Ѵ�.
			// �װ� �ƴ϶�� TimeManager�� �� �Լ��� ȣ���ϴ°� �ȴ�. ���� �̺�Ʈ��� ������ �� �����ѵ�...
			// ó���� �����ߴ� �� ó�� Runtime���� FixedTime�� �����ϴ� �͵� �����غ� �� �ִ�.
			// ������ �� ��쿡�� �ᱹ�� FixedTime�� FixedUpdate()�� �������ִ� ���� �ڿ�������.
			// �̸� Update()�� �������Ѻ��� �ᱹ �Ű������� ���ְ� �ʿ��� �� TimeManager ��ü�� ���� 
			// FixedTime�� ElapsedTime�� ȹ���ϴ� ���� ���� ����ϴ�. ����Ƽ������ �׷��� �����Ǿ� �ִ�.
			FixedUpdate();
			
			Update(m_deltaTime);
			// �Է� ����
			Render();
			
		}
		else
		{
			// �Է� ���۸� ����
		}

		// Present ����
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