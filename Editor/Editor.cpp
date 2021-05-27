#include "Editor.h"
#include "WidgetMenu.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include "External/ImGui/imgui_impl_win32.h"
#include "External/ImGui/imgui_impl_dx11.h"

namespace Editor
{
	Editor::Editor()
	{
	}

	Editor::~Editor()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	
	bool Editor::Initialize()
	{
		// ���⿡�� ini ���Ϸ� �����츦 �缳���� �� �ִ�.
		// �װ� �� ����� ���δ�.
		// ������ ũ�� ������ Backbuffer�� ������ ��ġ�Ƿ� �ý��� ���� ���� �ϴ°Ŵ�.
		{
			// �� ���ϰ�δ� ��� ������ �����ϴ� ���� ����.
			auto filePath = Dive::FileSystemHelper::GetWorkingDirectory() + "editor.ini";
			Dive::IniHelper ini(filePath);
			bool bMaximize	= ini["Window"]["bMaximize"] << false;
			int width		= ini["Window"]["Width"] << 800;
			int height		= ini["Window"]["Height"] << 600;
			int posX		= (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
			int posY		= (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

			// ������Ʈ���� �����ҰŶ�� �� ���� ��� ������ �����ϴ� ���� ����?
			// �ƴϴ�. ����Ʈ ���ڿ��� �ְ�(ini�� ������ �ʿ䵵 ����.)
			// Scene �ε� ���ο� ���� scene �̸��� ������ ����Ʈ �̸��� ���� �����ϴ� ���� ����.
			SetWindowText(m_hWnd, L"Test Text");

			if (!Dive::FileSystemHelper::FileExists(filePath))
			{
				ini["Window"]["Width"]		= width;
				ini["Window"]["Height"]		= height;
				ini["Window"]["bMaximize"]	= bMaximize;
			}

			RECT rcClient = { 0, 0, width, height };

			AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

			SetWindowPos(m_hWnd, nullptr, posX, posY, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top), 0);

			ShowWindow(m_hWnd, bMaximize ? SW_MAXIMIZE : SW_SHOW);
			UpdateWindow(m_hWnd);
		}

		if (!Dive::Runtime::Initialize())
			return false;

		// ImGui �ʱ�ȭ
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			ImGui::StyleColorsDark();

			ImGui_ImplWin32_Init(m_hWnd);

			auto pDevice = Dive::Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
			auto pImmediateContext = Dive::Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();
			ImGui_ImplDX11_Init(pDevice, pImmediateContext);
		}

		// Widget ����
		{
			m_widgets.emplace_back(std::make_shared<WidgetMenu>());
		}

		// ��� �ʱ�ȭ�� ����ģ �� ������ show?

		APP_TRACE("Editor�� �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void Editor::Render()
	{
		auto graphicsDevice = Dive::Renderer::GetInstance().GetGraphicsDevice();
		graphicsDevice->PresentBegin();

		// ImGui���� �׸��� �κ�
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// �� ������ Widget�� �׸� ���̴�.
			for (auto pWidget : m_widgets)
			{
				pWidget->Begin();
				pWidget->Tick();
				pWidget->End();
			}

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		// �̰� �������� �׸��� �κ�
		// �����Ϳ����� RenderPath3D�� �����ؾ� �Ѵ�.
		// Debug�� Render�� �߰��ǰ�,
		// Scene�� �ٸ� RenderTarget�� �׷����� �ϱ� �����̴�.
		if (m_activePath)
		{
			m_activePath->Render();
		}

		Compose();
		graphicsDevice->PresentEnd();
	}

	void Editor::ResizeWindow(unsigned int width, unsigned int height)
	{
		auto filePath = Dive::FileSystemHelper::GetWorkingDirectory() + "editor.ini";
		Dive::IniHelper ini(filePath);
		ini["Window"]["Width"]		= width;
		ini["Window"]["Height"]		= height;
		ini["Window"]["bMaximize"]	= IsZoomed(m_hWnd) ? true : false;
	}
}