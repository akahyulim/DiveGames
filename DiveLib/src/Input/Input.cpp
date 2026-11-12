#include "Input/Input.h"
#include "Core/Common.h"
#include "Core/LogManager.h"

using Microsoft::WRL::ComPtr;

namespace Dive
{
	Input::Input(HWND hWnd)
		: m_hWnd(hWnd)
	{
		DV_LOG(Input, info, "Initializing DirectInput8 System (HWND: {})...", static_cast<void*>(hWnd));

		if (!::IsWindow(hWnd))
		{
			DV_LOG(Input, err, "Invalid window handle provided for Input system.");
			return;
		}

		HINSTANCE hInst = ::GetModuleHandle(nullptr);
		auto hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)m_directInput.GetAddressOf(), nullptr);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "Failed to create DirectInput8 interface: {}", ErrorUtils::ToVerbose(hr));
			return;
		}
		DV_LOG(Input, info, "DirectInput8 interface created successfully.");

		// 키보드 생성 및 설정
		hr = m_directInput->CreateDevice(GUID_SysKeyboard, m_keyboard.GetAddressOf(), nullptr);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "Failed to create keyboard device: {}", ErrorUtils::ToVerbose(hr));
			return;
		}

		hr = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "Failed to set keyboard data format: {}", ErrorUtils::ToVerbose(hr));
			return;
		}

		hr = m_keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "Failed to set keyboard cooperative level: {}", ErrorUtils::ToVerbose(hr));
			return;
		}

		hr = m_keyboard->Acquire();
		if (FAILED(hr))
		{
			DV_LOG(Input, warn, "Initial keyboard acquire failed (will retry on update): {}", ErrorUtils::ToVerbose(hr));
		}
		else
		{
			DV_LOG(Input, info, "Keyboard device initialized and acquired successfully.");
		}

		// 마우스 생성 및 설정
		hr = m_directInput->CreateDevice(GUID_SysMouse, m_mouse.GetAddressOf(), NULL);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "Failed to create mouse device: {}", ErrorUtils::ToVerbose(hr));
			return;
		}

		hr = m_mouse->SetDataFormat(&c_dfDIMouse);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "Failed to set mouse data format: {}", ErrorUtils::ToVerbose(hr));
			return;
		}

		hr = m_mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "Failed to set mouse cooperative level: {}", ErrorUtils::ToVerbose(hr));
			return;
		}

		hr = m_mouse->Acquire();
		if (FAILED(hr))
		{
			DV_LOG(Input, warn, "Initial mouse acquire failed (will retry on update): {}", ErrorUtils::ToVerbose(hr));
		}
		else
		{
			DV_LOG(Input, info, "Mouse device initialized and acquired successfully.");
		}

		DV_LOG(Input, info, "Input system initialization completed.");
	}

	Input::~Input()
	{
		DV_LOG(Input, info, "Shutting down Input system...");

		if (m_mouse)
		{
			m_mouse->Unacquire();
			m_mouse.Reset();
		}

		if (m_keyboard)
		{
			m_keyboard->Unacquire();
			m_keyboard.Reset();
		}

		m_directInput.Reset();
		DV_LOG(Input, info, "Input system shut down successfully.");
	}

	void Input::Update()
	{
		memcpy(m_oldKeyStates, m_keyStates, sizeof(m_keyStates));
		memcpy(m_oldMouseButtons, m_mouseButtons, sizeof(m_mouseButtons));

		if (!ReadKeyboard()) return;
		if (!ReadMouse()) return;

		for (int i = 0; i < MAX_NUM_BUTTONS; ++i)
			m_mouseButtons[i] = m_mouseState.rgbButtons[i] ? 1 : 0;
	}

	bool Input::ReadKeyboard()
	{
		auto hr = m_keyboard->GetDeviceState(sizeof(m_keyStates), static_cast<LPVOID>(&m_keyStates));
		if (FAILED(hr))
		{
			if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			{
				DV_LOG(Input, warn, "Keyboard focus lost. Re-acquiring device...");
				m_keyboard->Acquire();
			}
			else
			{
				DV_LOG(Input, err, "Failed to read keyboard device state: {}", ErrorUtils::ToVerbose(hr));
				return false;
			}
		}

		return true;
	}

	bool Input::ReadMouse()
	{
		auto hr = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), static_cast<LPVOID>(&m_mouseState));
		if (FAILED(hr))
		{
			if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			{
				DV_LOG(Input, warn, "Mouse focus lost. Re-acquiring device...");
				m_mouse->Acquire();
			}
			else
			{
				DV_LOG(Input, err, "Failed to read mouse device state: {}", ErrorUtils::ToVerbose(hr));
				return false;
			}
		}

		return true;
	}

	bool Input::KeyState(int key)
	{
		return m_keyStates[key] & 0X80;
	}

	bool Input::KeyDown(int key)
	{
		return (m_keyStates[key] & 0X80) && !(m_oldKeyStates[key] & 0X80);
	}

	bool Input::KeyUp(int key)
	{
		return !(m_keyStates[key] & 0X80) && (m_oldKeyStates[key] & 0X80);
	}

	bool Input::KeyPress(int key)
	{
		return (m_keyStates[key] & 0X80) && (m_oldKeyStates[key] & 0X80);
	}

	bool Input::MouseButtonState(int btn)
	{
		return m_mouseButtons[btn] == 1;
	}

	bool Input::MouseButtonDown(int btn)
	{
		return (m_mouseButtons[btn] == 1) && !(m_oldMouseButtons[btn] == 1);
	}

	bool Input::MouseButtonUp(int btn)
	{
		return !(m_mouseButtons[btn] == 1) && (m_oldMouseButtons[btn] == 1);
	}

	bool Input::MouseButtonPress(int btn)
	{
		return (m_mouseButtons[btn] == 1) && (m_oldMouseButtons[btn] == 1);
	}

	DirectX::XMUINT2 Input::GetMousePosition()
	{
		return GetMousePosition(m_hWnd);
	}

	DirectX::XMUINT2 Input::GetMousePosition(HWND hWnd)
	{
		assert(hWnd);

		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);

		RECT clientRect;
		GetClientRect(hWnd, &clientRect);

		int32_t x = std::clamp(point.x, 0L, clientRect.right);
		int32_t y = std::clamp(point.y, 0L, clientRect.bottom);

		return DirectX::XMUINT2(static_cast<uint32_t>(x), static_cast<uint32_t>(y));
	}

	DirectX::XMFLOAT2 Input::GetMouseMoveDelta()
	{
		return { static_cast<float>(m_mouseState.lX), static_cast<float>(m_mouseState.lY) };
	}

	int Input::GetMouseWheelDelta()
	{
		return static_cast<int>(m_mouseState.lZ);
	}
}
