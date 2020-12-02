#include "DivePch.h"
#include "Graphics.h"
#include "Window.h"
#include "Core/Context.h"


namespace Dive
{
	Graphics::Graphics(Context* context)
		: Subsystem(context)

	{
		m_window = std::make_shared<Window>();
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

		// ��۸� �����ʹ� ���....?
		if (!m_window->Run())
			return false;

		return true;
	}

	void Graphics::EndFrame()
	{
	}

	bool Graphics::IsInitialized()
	{
		// device ���� Ȯ�ε� �ʿ�
		return m_window != nullptr;
	}

	bool Graphics::SetScreenMode()
	{
		// ������ ����
		if (!m_window->Create(1024, 768))
			return false;
		m_window->Show();

		// swap chain?

		// rhi device

		return true;
	}
}
