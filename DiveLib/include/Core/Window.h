#pragma once
#include <Windows.h>
#include <string>
#include "Utilities/Delegate.h"

namespace Dive
{
	struct WindowEventData
	{
		void* handle = nullptr;
		uint32_t msg = 0;
		uint64_t wParam = 0;
		uint64_t lParam = 0;
		float width = 0.0f;
		float height = 0.0f;
	};

	DECLARE_EVENT(WindowEvent, Window, const WindowEventData&);
	DECLARE_EVENT(ResizedEvent, Window, uint32_t, uint32_t);

	class Window
	{
		friend LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		Window(int width, int height, const wchar_t* title);
		~Window();

		LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		bool Run();
		void PollEvents();

		void SetMessageCallback(LONG_PTR callBack) const;

		bool Resize(uint32_t width, uint32_t height) const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		HWND GetWindowHandle() const { return m_hWnd; }

		bool IsActive() const;

		void Show() const;
		void Hide() const;
		void Close() const;

		bool IsWindowed() const;
		void FullScreen() const;

		void SetWindowed(bool windowed = true) const;

		void Minimize() const;
		void Maximize() const;
		void Restore() const;
		bool IsMaximize() const;

		bool ShouldClose() const { return m_shouldClose; }

		void SetTitle(const std::string& title) const;

		WindowEvent& GetWindowEvent() { return m_windowEvent; }
		ResizedEvent& GetResizedEvent() { return m_resizedEvent; }

	private:
		void broadcastEvents(const WindowEventData& data);

	private:
		HWND m_hWnd{};
		bool m_shouldClose = false;
		bool m_isResizing = false;

		WindowEvent m_windowEvent;
		ResizedEvent m_resizedEvent;

	};
}