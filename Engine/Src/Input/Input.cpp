#include "DivePch.h"
#include "Input.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"

namespace Dive
{
	static constexpr uint16_t MAX_NUM_KEYS = 256;
	static constexpr uint8_t MAX_NUM_BUTTONS = 4;

	static IDirectInput8* s_pDirectInput = nullptr;
	static IDirectInputDevice8* s_pKeyboard = nullptr;
	static IDirectInputDevice8* s_pMouse = nullptr;

	static uint8_t s_KeyStates[MAX_NUM_KEYS] = { 0, };
	static uint8_t s_OldKeyStates[MAX_NUM_KEYS] = { 0, };

	static DIMOUSESTATE s_MouseState;

	static uint8_t s_MouseButtons[MAX_NUM_BUTTONS] = { 0, };
	static uint8_t s_OldMouseButtons[MAX_NUM_BUTTONS] = { 0, };
	static DirectX::XMFLOAT2 s_MousePosition = { 0.0f, 0.0f };
	static DirectX::XMFLOAT2 s_OldMousePosition = { 0.0f, 0.0f };
	static DirectX::XMFLOAT2 s_MouseDelta = { 0.0f, 0.0f };

	static bool s_MouseIsInValidRect = false;

	bool Input::Initialize(HINSTANCE hInstance, HWND hWnd)
	{
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&s_pDirectInput, nullptr)))
		{
			DV_CORE_ERROR("DirectInput ��ü ������ �����Ͽ����ϴ�.");
			return false;
		}

		// Ű���� ����
		{
			if (FAILED(s_pDirectInput->CreateDevice(GUID_SysKeyboard, &s_pKeyboard, nullptr)))
			{
				DV_CORE_ERROR("Ű���� ��ġ ��ü ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(s_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
			{
				DV_CORE_ERROR("Ű���� ������ ���� ������ �����Ͽ����ϴ�.");
				return false;
			}

			// �ϴ� ������ �����ߴ�.
			if (FAILED(s_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
			{
				DV_CORE_ERROR("Ű���� ���۷���Ʈ ���� ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(s_pKeyboard->Acquire()))
			{
				DV_CORE_ERROR("Ű���� ȹ�濡 �����Ͽ����ϴ�.");
				return false;
			}
		}

		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&s_pDirectInput, NULL)))
		{
			DV_CORE_ERROR("DirectInput ��ü ������ �����Ͽ����ϴ�.");
			return false;
		}

		// ���콺 ����
		{
			if (FAILED(s_pDirectInput->CreateDevice(GUID_SysMouse, &s_pMouse, NULL)))
			{
				DV_CORE_ERROR("���콺 ��ġ ��ü ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(s_pMouse->SetDataFormat(&c_dfDIMouse)))
			{
				DV_CORE_ERROR("���콺 ������ ���� ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(s_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
			{
				DV_CORE_ERROR("���콺 ���۷���Ʈ ���� ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(s_pMouse->Acquire()))
			{
				DV_CORE_ERROR("���콺 ȹ�濡 �����Ͽ����ϴ�.");
				return false;
			}
		}

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
	}

	void Input::Update()
	{
		ReadKeyboard();
		ReadMouse();
	}

	void Input::ReadKeyboard()
	{
		if (!s_pDirectInput && !s_pKeyboard)
			return;

		memcpy(s_OldKeyStates, s_KeyStates, sizeof(s_KeyStates));
		
		HRESULT hResult = s_pKeyboard->GetDeviceState(sizeof(s_KeyStates), static_cast<LPVOID>(&s_KeyStates));
		if (FAILED(hResult))
		{
			ZeroMemory(&s_KeyStates, sizeof(s_KeyStates));

			if ((hResult == DIERR_INPUTLOST) || (hResult == DIERR_NOTACQUIRED))
				s_pKeyboard->Acquire();
			else
			{
				DV_CORE_ASSERT("Ű���� ���ῡ �����Ͽ����ϴ�.");
				return;
			}
		}
	}

	void Input::ReadMouse()
	{
		if (!s_pDirectInput && !s_pMouse)
			return;

		memcpy(s_OldMouseButtons, s_MouseButtons, sizeof(s_MouseButtons));
		s_OldMousePosition = s_MousePosition;

		HRESULT hResult = s_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), static_cast<LPVOID>(&s_MouseState));
		if (FAILED(hResult))
		{
			if ((hResult == DIERR_INPUTLOST) || (hResult == DIERR_NOTACQUIRED))
				s_pMouse->Acquire();
			else
			{
				DV_CORE_ASSERT("���콺 ���ῡ �����Ͽ����ϴ�.");
				return;
			}
		}

		for (int i = 0; i < MAX_NUM_BUTTONS; ++i)
			s_MouseButtons[i] = s_MouseState.rgbButtons[i] ? 1 : 0;

		s_MousePosition.x += static_cast<float>(s_MouseState.lX);
		s_MousePosition.y += static_cast<float>(s_MouseState.lY);

		/*
		// ���� �̺�Ʈ�� ���� ��������.
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