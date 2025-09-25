#include "stdafx.h"
#include "Input.h"
#include "Window.h"

using Microsoft::WRL::ComPtr;

namespace Dive
{
	Microsoft::WRL::ComPtr<IDirectInput8> Input::s_directInput;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> Input::s_keyboard;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> Input::s_mouse;

	uint8_t Input::s_keyStates[MAX_NUM_KEYS]{};
	uint8_t Input::s_oldKeyStates[MAX_NUM_KEYS]{};

	DIMOUSESTATE Input::s_mouseState{};

	uint8_t Input::s_mouseButtons[MAX_NUM_BUTTONS]{};
	uint8_t Input::s_oldMouseButtons[MAX_NUM_BUTTONS]{};

	bool Input::Initialize()
	{
		HINSTANCE hInstance = Window::GetInstanceHandle();
		HWND hWnd = Window::GetWindowHandle();

		auto hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)s_directInput.GetAddressOf(), nullptr);
		if(FAILED(hr))
		{
			DV_LOG(Input, err, "[::Initialize] DirectInput8Create 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		// 키보드 생성
		hr = s_directInput->CreateDevice(GUID_SysKeyboard, s_keyboard.GetAddressOf(), nullptr);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "[::Initialize] 키보드 CreateDevice 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = s_keyboard->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "[::Initialize] 키보드 SetDataFormat 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = s_keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "[::Initialize] 키보드 SetCooperativeLevel 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = s_keyboard->Acquire();
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "[::Initialize] 키보드 Acquire 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		// 마우스 생성
		hr = s_directInput->CreateDevice(GUID_SysMouse, s_mouse.GetAddressOf(), NULL);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "[::Initialize] 마우스 CreateDevice 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = s_mouse->SetDataFormat(&c_dfDIMouse);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "[::Initialize] 마우스 SetDataFormat 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = s_mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "[::Initialize] 마우스 SetCooperativeLevel 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = s_mouse->Acquire();
		if (FAILED(hr))
		{
			DV_LOG(Input, err, "[::Initialize] 마우스 Acquire 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		DV_SUBSCRIBE_EVENT(eEventType::BeginFrame, DV_EVENT_HANDLER_STATIC(OnBeginFrame));

		DV_LOG(Input, info, "초기화 성공");
		return true;
	}

	void Input::Shutdown()
	{
		if (s_mouse)
		{
			s_mouse->Unacquire();
			s_mouse.Reset();
		} 
		
		if(s_keyboard)
		{
			s_keyboard->Unacquire();
			s_keyboard.Reset();
		}
		
		s_directInput.Reset();

		DV_LOG(Input, info, "셧다운 완료");
	}

	void Input::OnBeginFrame()
	{	
		// urho3d는 input begin/end 이벤트로 감싼다.
		Update();
	}

	void Input::Update()
	{
		if (!ReadKeyboard())
		{
			DV_LOG(Input, err, "[::Update] 키보드 입력 읽기 실패");
			return;
		}

		if (!ReadMouse())
		{
			DV_LOG(Input, err, "[::Update] 마우스 입력 읽기 실패");
			return;
		}

		ProcessInput();
	}

	bool Input::ReadKeyboard()
	{
		auto hr = s_keyboard->GetDeviceState(sizeof(s_keyStates), static_cast<LPVOID>(&s_keyStates));
		if (FAILED(hr))
		{
			if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
				s_keyboard->Acquire();
			else
			{
				DV_LOG(Input, err, "[::ReadKeyboard] 키보드 연결 실패: {}", ErrorUtils::ToVerbose(hr));
				return false;
			}
		}

		return true;
	}
	
	bool Input::ReadMouse()
	{
		auto hr = s_mouse->GetDeviceState(sizeof(DIMOUSESTATE), static_cast<LPVOID>(&s_mouseState));
		if (FAILED(hr))
		{
			if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
				s_mouse->Acquire();
			else
			{
				DV_LOG(Input, err, "[::ReadMouse] 마우스 연결 실패: {}", ErrorUtils::ToVerbose(hr));
				return false;
			}
		}

		return true;
	}

	void Input::ProcessInput()
	{
		memcpy(s_oldKeyStates, s_keyStates, sizeof(s_keyStates));
		memcpy(s_oldMouseButtons, s_mouseButtons, sizeof(s_mouseButtons));

		for (int i = 0; i < MAX_NUM_BUTTONS; ++i)
			s_mouseButtons[i] = s_mouseState.rgbButtons[i] ? 1 : 0;
	}
	
	bool Input::KeyState(int key)
	{
		return s_keyStates[key] & 0X80;
	}
	
	bool Input::KeyDown(int key)
	{
		return (s_keyStates[key] & 0X80) && !(s_oldKeyStates[key] & 0X80);
	}
	
	bool Input::KeyUp(int key)
	{
		return !(s_keyStates[key] & 0X80) && (s_oldKeyStates[key] & 0X80);
	}
	
	bool Input::KeyPress(int key)
	{
		return (s_keyStates[key] & 0X80) && (s_oldKeyStates[key] & 0X80);
	}

	bool Input::MouseButtonState(int btn)
	{
		return s_mouseButtons[btn] == 1;
	}
	
	bool Input::MouseButtonDown(int btn)
	{
		return (s_mouseButtons[btn] == 1) && !(s_oldMouseButtons[btn] == 1);
	}
	
	bool Input::MouseButtonUp(int btn)
	{
		return !(s_mouseButtons[btn] == 1) && (s_oldMouseButtons[btn] == 1);
	}
	
	bool Input::MouseButtonPress(int btn)
	{
		return (s_mouseButtons[btn] == 1) && (s_oldMouseButtons[btn] == 1);
	}
	
	DirectX::XMUINT2 Input::GetMousePosition()
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(Window::GetWindowHandle(), &point);

		return DirectX::XMUINT2(static_cast<uint32_t>(point.x), static_cast<uint32_t>(point.y));
	}

	DirectX::XMFLOAT2 Input::GetMouseMoveDelta()
	{
		return { static_cast<float>(s_mouseState.lX), static_cast<float>(s_mouseState.lY) };
	}

	int Input::GetMouseWheelDelta()
	{
		return static_cast<int>(s_mouseState.lZ);
	}
}
