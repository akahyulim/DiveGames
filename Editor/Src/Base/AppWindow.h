#pragma once
#include <Windows.h>
#include <string>
#include <functional>

namespace Dive
{
	struct WindowProps;
	class Event;

	class AppWindow
	{
		using EventCallbackFn = std::function<void(Event&)>;

	public:
		AppWindow(HINSTANCE hInstance, const WindowProps& props);
		~AppWindow() = default;

		void Destroy();

		void Show();

		bool Run();

		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

		LRESULT CALLBACK AppWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		struct WindowData
		{
			HINSTANCE hInstance;
			HWND hWnd;

			UINT Msg;
			WPARAM wParam;
			LPARAM lParam;

			std::string Title;

			unsigned int Width;
			unsigned int Height;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}