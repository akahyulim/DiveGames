#include "stdafx.h"
#include "Input.h"
#include "CoreDefs.h"
#include "Window.h"

using Microsoft::WRL::ComPtr;

namespace Dive
{
	Microsoft::WRL::ComPtr<IDirectInput8> Input::s_DirectInput;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> Input::s_Keyboard;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> Input::s_Mouse;

	uint8_t Input::s_KeyStates[MAX_NUM_KEYS]{};
	uint8_t Input::s_OldKeyStates[MAX_NUM_KEYS]{};

	DIMOUSESTATE Input::s_MouseState{};

	uint8_t Input::s_MouseButtons[MAX_NUM_BUTTONS]{};
	uint8_t Input::s_OldMouseButtons[MAX_NUM_BUTTONS]{};
	DirectX::XMFLOAT2 Input::s_MousePosition{};
	DirectX::XMFLOAT2 Input::s_OldMousePosition{};
	DirectX::XMFLOAT2 Input::s_MouseDelta{};

	bool Input::s_MouseIsInValidRect;

	void Input::Initialize()
	{
		HINSTANCE hInstance = Window::GetInstanceHandle();
		HWND hWnd = Window::GetWindowHandle();

		assert(hInstance);
		assert(hWnd);
 
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)s_DirectInput.GetAddressOf(), nullptr))) 
		{
			DV_LOG(Input, critical, "DirectInput 객체 생성 실패!");
			return;
		}

		// 키보드 생성
		{
			if (FAILED(s_DirectInput->CreateDevice(GUID_SysKeyboard, s_Keyboard.GetAddressOf(), nullptr)))
			{
				DV_LOG(Input, critical, "키보드 장치 객체 생성 실패!");
				return;
			}

			if (FAILED(s_Keyboard->SetDataFormat(&c_dfDIKeyboard)))
			{
				DV_LOG(Input, critical, "키보드 데이터 포멧 설정 실패!");
				return;
			}

			if (FAILED(s_Keyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
			{
				DV_LOG(Input, critical, "키보드 코퍼레이트 레벨 설정 실패!");
				return;
			}

			if (FAILED(s_Keyboard->Acquire()))
			{
				DV_LOG(Input, critical, "키보드 획득 실패!");
				return;
			}
		}

		// 마우스 생성
		{
			if (FAILED(s_DirectInput->CreateDevice(GUID_SysMouse, s_Mouse.GetAddressOf(), NULL)))
			{
				DV_LOG(Input, critical, "마우스 장치 객체 생성 실패!");
				return;
			}

			if (FAILED(s_Mouse->SetDataFormat(&c_dfDIMouse)))
			{
				DV_LOG(Input, critical, "마우스 데이터 포멧 설정 실패!");
				return;
			}

			if (FAILED(s_Mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
			{
				DV_LOG(Input, critical, "마우스 코퍼레이트 레벨 설정 실패!");
				return;
			}

			if (FAILED(s_Mouse->Acquire()))
			{
				DV_LOG(Input, critical, "마우스 획득 실패!");
				return;
			}
		}

		DV_LOG(Input, info, "초기화 완료");
	}

	void Input::Shutdown()
	{
		if (s_Mouse)
		{
			s_Mouse->Unacquire();
			s_Mouse.Reset();
		} 
		
		if(s_Keyboard)
		{
			s_Keyboard->Unacquire();
			s_Keyboard.Reset();
		}
		
		s_DirectInput.Reset();

		DV_LOG(Input, info, "셧다운 완료");
	}

	void Input::Tick()
	{
		// 순서대로 처리하면 동시 입력은 불가능하지 않을까?
		ReadKeyboard();
		ReadMouse();
	}

	void Input::ReadKeyboard()
	{
		if (!s_DirectInput || !s_Keyboard)
			return;

		memcpy(s_OldKeyStates, s_KeyStates, sizeof(s_KeyStates));

		HRESULT result = s_Keyboard->GetDeviceState(sizeof(s_KeyStates), static_cast<LPVOID>(&s_KeyStates));
		if (FAILED(result))
		{
			ZeroMemory(&s_KeyStates, sizeof(s_KeyStates));

			if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
				s_Keyboard->Acquire();
			else
				DV_LOG(Input, err, "키보드 연결에 실패하였습니다.");
		}
	}
	
	void Input::ReadMouse()
	{
		if (!s_DirectInput || !s_Mouse)
			return;

		memcpy(s_OldMouseButtons, s_MouseButtons, sizeof(s_MouseButtons));
		s_OldMousePosition = s_MousePosition;

		HRESULT result = s_Mouse->GetDeviceState(sizeof(DIMOUSESTATE), static_cast<LPVOID>(&s_MouseState));
		if (FAILED(result))
		{
			if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
				s_Mouse->Acquire();
			else
			{
				DV_LOG(Input, err, "마우스 연결에 실패하였습니다.");
				return;
			}
		}

		for (int i = 0; i < MAX_NUM_BUTTONS; ++i)
			s_MouseButtons[i] = s_MouseState.rgbButtons[i] ? 1 : 0;

		s_MousePosition.x += static_cast<float>(s_MouseState.lX);
		s_MousePosition.y += static_cast<float>(s_MouseState.lY);

		/*
		// 추후 이벤트를 통해 갱신하자.
		DirectX::XMFLOAT2 windowSize = Renderer::GetResolutionRender();

		if (s_MousePosition.x < 0)
			s_MousePosition.x = 0;
		if (s_MousePosition.x > windowSize.x)
			s_MousePosition.x = windowSize.x;

		if (s_MousePosition.y < 0)
			s_MousePosition.y = 0;
		if (s_MousePosition.y > windowSize.y)
			s_MousePosition.y = windowSize.y;
		*/
		s_MouseDelta.x = s_MousePosition.x - s_OldMousePosition.x;
		s_MouseDelta.y = s_MousePosition.y - s_OldMousePosition.y;
	}
	
	bool Input::KeyState(int key)
	{
		return s_KeyStates[key] & 0X80;
	}
	
	bool Input::KeyDown(int key)
	{
		return (s_KeyStates[key] & 0X80) && !(s_OldKeyStates[key] & 0X80);
	}
	
	bool Input::KeyUp(int key)
	{
		return !(s_KeyStates[key] & 0X80) && (s_OldKeyStates[key] & 0X80);
	}
	
	bool Input::KeyPress(int key)
	{
		return (s_KeyStates[key] & 0X80) && (s_OldKeyStates[key] & 0X80);
	}

	bool Input::MouseButtonState(int btn)
	{
		return s_MouseButtons[btn] == 1;
	}
	
	bool Input::MouseButtonDown(int btn)
	{
		return (s_MouseButtons[btn] == 1) && !(s_OldMouseButtons[btn] == 1);
	}
	
	bool Input::MouseButtonUp(int btn)
	{
		return !(s_MouseButtons[btn] == 1) && (s_OldMouseButtons[btn] == 1);
	}
	
	bool Input::MouseButtonPress(int btn)
	{
		return (s_MouseButtons[btn] == 1) && (s_OldMouseButtons[btn] == 1);
	}
	
	const DirectX::XMFLOAT2& Input::GetMouseMoveDelta()
	{
		return s_MouseDelta;
	}
	
	const DirectX::XMFLOAT2& Input::GetMousePosition()
	{
		return s_MousePosition;
	}
	
	bool Input::GetMouseIsInValidRect()
	{
		return s_MouseIsInValidRect;
	}
	
	void Input::SetMouseIsInValidRect(bool valid)
	{
		s_MouseIsInValidRect = valid;
	}
}
