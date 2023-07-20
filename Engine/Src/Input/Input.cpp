#include "DivePch.h"
#include "Input.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "IO/Log.h"

namespace Dive
{
	static HWND s_hWnd = 0;

	static IDirectInput8* s_pDirectInput = nullptr;
	static IDirectInputDevice8* s_pKeyboard = nullptr;
	static IDirectInputDevice8* s_pMouse = nullptr;

	static DIMOUSESTATE s_MouseState;

	static uint8_t s_KeyStates[256] = { 0, };
	static uint8_t s_OldKeyStates[256] = { 0, };

	static uint8_t s_MouseButtons[4] = { 0, };
	static uint8_t s_OldMouseButtons[4] = { 0, };

	bool Input::Initialize(HINSTANCE hInstance, HWND hWnd)
	{
		s_hWnd = hWnd;

		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&s_pDirectInput, nullptr)))
		{
			DV_CORE_ERROR("Input::Initialize - DirectInput 객체 생성에 실패하였습니다.");
			return false;
		}

		if (FAILED(s_pDirectInput->CreateDevice(GUID_SysKeyboard, &s_pKeyboard, nullptr)))
		{
			DV_CORE_ERROR("Input::Initialize - 키보드 장치 객체 생성에 실패하였습니다.");
			return false;
		}

		if (FAILED(s_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		{
			DV_CORE_ERROR("Input::Initialize - 키보드 데이터 포멧 설정에 실패하였습니다.");
			return false;
		}

		if (FAILED(s_pKeyboard->SetCooperativeLevel(s_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
		{
			return false;
		}

		if (FAILED(s_pKeyboard->Acquire()))
		{
			return false;
		}

		if (FAILED(s_pDirectInput->CreateDevice(GUID_SysMouse, &s_pMouse, nullptr)))
		{
			DV_CORE_ERROR("Input::Initialize - 마우스 장치 객체 생성에 실패하였습니다.");
			return false;
		}

		if (FAILED(s_pMouse->SetDataFormat(&c_dfDIMouse)))
		{
			DV_CORE_ERROR("Input::Initialize - 마우스 데이터 포멧 설정에 실패하였습니다.");
			return false;
		}

		if (FAILED(s_pMouse->SetCooperativeLevel(s_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		{
			return false;
		}

		if (FAILED(s_pMouse->Acquire()))
		{
			return false;
		}

		DV_CORE_TRACE("Input 초기화에 성공하였습니다.");

		return true;
	}

	void Input::Shutdown()
	{
		if (s_pMouse)
		{
			s_pMouse->Unacquire();
			DV_RELEASE(s_pMouse);
		}

		if (s_pKeyboard)
		{
			s_pKeyboard->Unacquire();
			DV_RELEASE(s_pKeyboard);
		}

		DV_RELEASE(s_pDirectInput);

		DV_CORE_TRACE("Input 종료에 성공하였습니다.");
	}

	void Input::Update()
	{
		if (IsInitialized())
		{
			if (!readKeyboard())
				return;

			if (!readMouse())
				return;
		}
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

	bool Input::IsInitialized()
	{
		return (s_pDirectInput && s_pKeyboard && s_pMouse);
	}

	bool Input::readKeyboard()
	{
		memcpy(s_OldKeyStates, s_KeyStates, sizeof(s_KeyStates));
		ZeroMemory(s_KeyStates, sizeof(s_KeyStates));

		HRESULT hResult = s_pKeyboard->GetDeviceState(sizeof(s_KeyStates), (LPVOID)&s_KeyStates);
		if (FAILED(hResult))
		{
			ZeroMemory(&s_KeyStates, sizeof(s_KeyStates));

			if ((hResult == DIERR_INPUTLOST) || (hResult == DIERR_NOTACQUIRED))
				s_pKeyboard->Acquire();
			else
				return false;
		}

		return true;
	}

	bool Input::readMouse()
	{
		memcpy(s_OldMouseButtons, s_MouseButtons, sizeof(s_MouseButtons));
		ZeroMemory(&s_MouseButtons, sizeof(s_MouseButtons));

		HRESULT hResult = s_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&s_MouseState);
		if (FAILED(hResult))
		{
			if ((hResult == DIERR_INPUTLOST) || (hResult == DIERR_NOTACQUIRED))
				s_pMouse->Acquire();
			else
				return false;
		}

		for (int i = 0; i < 8; ++i)
			s_MouseButtons[i] = s_MouseState.rgbButtons[i] ? 1 : 0;

		return true;
	}
}