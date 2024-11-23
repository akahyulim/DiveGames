#include "DivePch.h"
#include "Input.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"

namespace Dive
{
	Input* Input::s_pInstance = nullptr;
	
	Input::Input()
		: m_pDirectInput(nullptr)
		, m_pKeyboard(nullptr)
		, m_pMouse(nullptr)
		, m_MousePosition({0.0f, 0.0f})
		, m_OldMousePosition({0.0f, 0.0f})
		, m_MouseDelta({0.0f, 0.0f})
		, m_MouseIsInValidRect(false)
	{
		for (uint16_t i = 0; i < MAX_NUM_KEYS; ++i)
		{
			m_KeyStates[i] = 0;
			m_OldKeyStates[i] = 0;
		}
		
		ZeroMemory(&m_MouseState, sizeof(m_MouseState));

		for (uint8_t i = 0; i < MAX_NUM_BUTTONS; ++i)
		{
			m_MouseButtons[i] = 0;
			m_OldMouseButtons[i] = 0;
		}
	}

	Input::~Input()
	{
		Shutdown();
	}

	bool Input::Initialize()
	{
		if (!Graphics::GetInstance()->IsInitialized())
		{
			DV_LOG(Input, err, "�����찡 �������� �ʾ� ��ǲ �ý����� �ʱ�ȭ�� �� �����ϴ�.");
			return false;
		}

		HINSTANCE hInstance = Graphics::GetInstance()->GetWindowInstance();
		HWND hWnd = Graphics::GetInstance()->GetWindowHandle();

		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, nullptr)))
		{
			DV_LOG(Input, err, "Input �ʱ�ȭ ���� �� DirectInput ��ü ������ �����Ͽ����ϴ�.");
			return false;
		}

		// Ű���� ����
		{
			if (FAILED(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
			{
				DV_LOG(Input, err, "Input �ʱ�ȭ ���� �� Ű���� ��ġ ��ü ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
			{
				DV_LOG(Input, err, "Input �ʱ�ȭ ���� �� Ű���� ������ ���� ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
			{
				DV_LOG(Input, err, "Input �ʱ�ȭ ���� �� Ű���� ���۷���Ʈ ���� ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(m_pKeyboard->Acquire()))
			{
				DV_LOG(Input, err, "Input �ʱ�ȭ ���� �� Ű���� ȹ�濡 �����Ͽ����ϴ�.");
				return false;
			}
		}

		// ���콺 ����
		{
			if (FAILED(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL)))
			{
				DV_LOG(Input, err, "Input �ʱ�ȭ ���� �� ���콺 ��ġ ��ü ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
			{
				DV_LOG(Input, err, "Input �ʱ�ȭ ���� �� ���콺 ������ ���� ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
			{
				DV_LOG(Input, err, "Input �ʱ�ȭ ���� �� ���콺 ���۷���Ʈ ���� ������ �����Ͽ����ϴ�.");
				return false;
			}

			if (FAILED(m_pMouse->Acquire()))
			{
				DV_LOG(Input, err, "Input �ʱ�ȭ ���� �� ���콺 ȹ�濡 �����Ͽ����ϴ�.");
				return false;
			}
		}

		DV_LOG(Input, trace, "��ǲ �ý��� �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void Input::Shutdown()
	{
		if (m_pMouse)
		{
			m_pMouse->Unacquire();
			DV_RELEASE(m_pMouse);
		}

		if (m_pKeyboard)
		{
			m_pKeyboard->Unacquire();
			DV_RELEASE(m_pKeyboard);
		}

		DV_RELEASE(m_pDirectInput);

		DV_LOG(Input, trace, "��ǲ �ý��� �˴ٿ �����Ͽ����ϴ�.");
	}

	void Input::Update()
	{
		ReadKeyboard();
		ReadMouse();
	}

	void Input::ReadKeyboard()
	{
		if (!m_pDirectInput && !m_pKeyboard)
			return;

		memcpy(m_OldKeyStates, m_KeyStates, sizeof(m_KeyStates));

		HRESULT hResult = m_pKeyboard->GetDeviceState(sizeof(m_KeyStates), static_cast<LPVOID>(&m_KeyStates));
		if (FAILED(hResult))
		{
			ZeroMemory(&m_KeyStates, sizeof(m_KeyStates));

			if ((hResult == DIERR_INPUTLOST) || (hResult == DIERR_NOTACQUIRED))
				m_pKeyboard->Acquire();
			else
			{
				DV_ASSERT(Input, false, "Ű���� ���ῡ �����Ͽ����ϴ�.");
				return;
			}
		}
	}

	void Input::ReadMouse()
	{
		if (!m_pDirectInput && !m_pMouse)
			return;

		memcpy(m_OldMouseButtons, m_MouseButtons, sizeof(m_MouseButtons));
		m_OldMousePosition = m_MousePosition;

		HRESULT hResult = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), static_cast<LPVOID>(&m_MouseState));
		if (FAILED(hResult))
		{
			if ((hResult == DIERR_INPUTLOST) || (hResult == DIERR_NOTACQUIRED))
				m_pMouse->Acquire();
			else
			{
				DV_ASSERT(Input, "���콺 ���ῡ �����Ͽ����ϴ�.");
				return;
			}
		}

		for (int i = 0; i < MAX_NUM_BUTTONS; ++i)
			m_MouseButtons[i] = m_MouseState.rgbButtons[i] ? 1 : 0;

		m_MousePosition.x += static_cast<float>(m_MouseState.lX);
		m_MousePosition.y += static_cast<float>(m_MouseState.lY);

		/*
		// ���� �̺�Ʈ�� ���� ��������.
		DirectX::XMFLOAT2 windowSize = Renderer::GetResolutionRender();

		if (m_MousePosition.x < 0)
			m_MousePosition.x = 0;
		if (m_MousePosition.x > windowSize.x)
			m_MousePosition.x = windowSize.x;

		if (m_MousePosition.y < 0)
			m_MousePosition.y = 0;
		if (m_MousePosition.y > windowSize.y)
			m_MousePosition.y = windowSize.y;
		*/
		m_MouseDelta.x = m_MousePosition.x - m_OldMousePosition.x;
		m_MouseDelta.y = m_MousePosition.y - m_OldMousePosition.y;
	}

	bool Input::KeyState(int key)
	{
		return m_KeyStates[key] & 0X80;
	}

	bool Input::KeyDown(int key)
	{
		return (m_KeyStates[key] & 0X80) && !(m_OldKeyStates[key] & 0X80);
	}

	bool Input::KeyUp(int key)
	{
		return !(m_KeyStates[key] & 0X80) && (m_OldKeyStates[key] & 0X80);
	}

	bool Input::KeyPress(int key)
	{
		return (m_KeyStates[key] & 0X80) && (m_OldKeyStates[key] & 0X80);
	}

	bool Input::MouseButtonState(int btn)
	{
		return m_MouseButtons[btn] == 1;
	}

	bool Input::MouseButtonDown(int btn)
	{
		return (m_MouseButtons[btn] == 1) && !(m_OldMouseButtons[btn] == 1);
	}

	bool Input::MouseButtonUp(int btn)
	{
		return !(m_MouseButtons[btn] == 1) && (m_OldMouseButtons[btn] == 1);
	}

	bool Input::MouseButtonPress(int btn)
	{
		return (m_MouseButtons[btn] == 1) && (m_OldMouseButtons[btn] == 1);
	}

	const DirectX::XMFLOAT2& Input::GetMouseMoveDelta()
	{
		return m_MouseDelta;
	}

	const DirectX::XMFLOAT2& Input::GetMousePosition()
	{
		return m_MousePosition;
	}

	bool Input::GetMouseIsInValidRect()
	{
		return m_MouseIsInValidRect;
	}

	void Input::SetMouseIsInValidRect(bool valid)
	{
		m_MouseIsInValidRect = valid;
	}

	Input* GetInput()
	{
		return Input::GetInstance();
	}
}