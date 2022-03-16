#include "divepch.h"
#include "Input.h"
#include "Events/WindowEvent.h"
#include "Base/Log.h"

namespace Dive
{
	HWND Input::m_WindowHandle = NULL;

	std::array<bool, 256> Input::m_Keys;
	std::array<bool, 256> Input::m_OldKeys;

	DirectX::XMFLOAT2 Input::m_MousePosition = DirectX::XMFLOAT2(0.0f, 0.0f);
	DirectX::XMFLOAT2 Input::m_MouseDelta = DirectX::XMFLOAT2(0.0f, 0.0f);
	float Input::m_MouseWheelDelta = 0.0f;

	bool Input::m_bGamePadConnected = false;

	bool Input::m_bNewFrame = false;
	bool Input::m_bDeviceChange = false;

	void Input::Initialize(HWND hWnd)
	{
		m_WindowHandle = hWnd;

		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01;
		rid.usUsage = 0x02;
		rid.dwFlags = RIDEV_INPUTSINK;
		rid.hwndTarget = m_WindowHandle;
		RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));

		SUBSCRIBE_EVENT(WindowDataEvent::s_Type, EVENT_HANDLER_STATIC(OnWindowData));
	}

	void Input::Update(float elapsedTime)
	{
		// 패드는 여기서 처리한다.
	}

	void Input::OnWindowData(const Event& e)
	{
		if (m_bNewFrame)
		{
			m_OldKeys = m_Keys;
			m_MouseDelta = DirectX::XMFLOAT2(0.0f, 0.0f);
			m_bDeviceChange = false;
		}

		const auto& windowDataEvent = dynamic_cast<const WindowDataEvent&>(e);
		const auto& windowData = windowDataEvent.GetWindowData();

		for (int i = 0; i != 255; i++)
		{
			m_Keys[i] = (::GetKeyState(i) & 0x8000) != 0;
		}

		// mouse
		{
			// current cursor position
			if (m_WindowHandle == ::GetActiveWindow())
			{
				POINT mousePos;
				if (::GetCursorPos(&mousePos))
				{
					::ScreenToClient(m_WindowHandle, &mousePos);
					m_MousePosition.x = static_cast<float>(mousePos.x);
					m_MousePosition.y = static_cast<float>(mousePos.y);
				}
			}

			// mouse delta
			if (windowData.msg == WM_INPUT)
			{
				UINT dwSize = 48;
				static BYTE lpb[48];

				GetRawInputData((HRAWINPUT)windowData.lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
				RAWINPUT* pRaw = (RAWINPUT*)lpb;

				if (pRaw->header.dwType == RIM_TYPEMOUSE)
				{
					m_MouseDelta.x = static_cast<float>(pRaw->data.mouse.lLastX);
					m_MouseDelta.y = static_cast<float>(pRaw->data.mouse.lLastY);
				}
			}

			// wheel delta
			{
				m_MouseWheelDelta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(windowData.wParam)) / static_cast<float>(WHEEL_DELTA);
			}
		}

		if (windowData.msg == WM_DEVICECHANGE)
		{
			m_bDeviceChange = true;
		}

		m_bNewFrame = false;
	}
	
	bool Input::GetKey(int key)
	{
		return m_Keys[key];
	}
	
	bool Input::KeyDown(int key)
	{
		return GetKey(key) && !m_OldKeys[key];
	}
	
	bool Input::KeyUp(int key)
	{
		return !GetKey(key) && m_OldKeys[key];
	}

	bool Input::KeyPress(int key)
	{
		return GetKey(key) && m_OldKeys[key];
	}

	void Input::SetMousePosition(const DirectX::XMFLOAT2& pos)
	{
		SetMousePosition(pos.x, pos.y);
	}
	
	void Input::SetMousePosition(float x, float y)
	{
		if (m_WindowHandle == GetActiveWindow())
		{
			POINT pt = POINT{ static_cast<LONG>(x), static_cast<LONG>(y) };
			if (::ClientToScreen(m_WindowHandle, &pt))
			{
				::SetCursorPos(pt.x, pt.y);
			}
		}
	}
}