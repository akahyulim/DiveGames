#include "DivePch.h"
#include "Graphics.h"
#include "Window.h"
#include "Core/Dive_Context.h"
#include "Core/Log.h"


namespace Dive
{
	Graphics::Graphics(Dive_Context* context)
		: Dive_Object(context)

	{
		m_window = std::make_shared<Window>();

		//SubscribeEvent(0, DIVE_HANDLER(Graphics, Update));
	}

	Graphics::~Graphics()
	{
		m_window->Destroy();
		m_window.reset();
	}

	bool Graphics::BeginFrame()
	{
		if (!IsInitialized())
			return false;

		// 댕글링 포인터는 어떻게....?
		if (!m_window->Run())
			return false;

		return true;
	}

	void Graphics::EndFrame()
	{
	}

	void Graphics::Update(size_t eventType)
	{
		CORE_TRACE("Graphics - Event Received!!!");
	}

	bool Graphics::IsInitialized()
	{
		// device 존재 확인도 필요
		return m_window != nullptr;
	}

	bool Graphics::SetScreenMode()
	{
		// 윈도우 생성
		if (!m_window->Create(1024, 768))
			return false;
		m_window->Show();

		// swap chain?

		// rhi device

		return true;
	}
}
