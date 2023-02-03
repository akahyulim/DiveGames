#include "divepch.h"
#include "Input.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Engine/EngineEvents.h"
#include "IO/Log.h"

namespace Dive
{
	Input::Input(Context* pContext)
		: Object(pContext),
		m_pDirectInput(nullptr),
		m_pKeyboard(nullptr),
		m_pMouse(nullptr),
		m_ScreenWidth(0),
		m_ScreenHeight(0)		
	{
		ZeroMemory(&m_KeyStates, sizeof(m_KeyStates));
		ZeroMemory(&m_OldKeyStates, sizeof(m_OldKeyStates));
		ZeroMemory(&m_MouseButtons, sizeof(m_MouseButtons));
		ZeroMemory(&m_OldMouseButtons, sizeof(m_OldMouseButtons));
	}

	Input::~Input()
	{
		DV_LOG_ENGINE_TRACE("Input 소멸 완료");
	}

	bool Input::Initialize(HINSTANCE hInstance, HWND hWnd, int width, int height)
	{
		m_WindowHandle = hWnd;
		m_ScreenWidth = width;
		m_ScreenHeight = height;

		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, nullptr)))
		{
			DV_LOG_ENGINE_ERROR("Input::Initialize - DirectInput 객체 생성에 실패하였습니다.");
			return false;
		}

		if (FAILED(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		{
			DV_LOG_ENGINE_ERROR("Input::Initialize - 키보드 장치 객체 생성에 실패하였습니다.");
			return false;
		}

		if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		{
			DV_LOG_ENGINE_ERROR("Input::Initialize - 키보드 데이터 포멧 설정에 실패하였습니다.");
			return false;
		}

		if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
		{
			return false;
		}

		if (FAILED(m_pKeyboard->Acquire()))
		{
			return false;
		}

		if (FAILED(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		{
			DV_LOG_ENGINE_ERROR("Input::Initialize - 마우스 장치 객체 생성에 실패하였습니다.");
			return false;
		}

		if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		{
			DV_LOG_ENGINE_ERROR("Input::Initialize - 마우스 데이터 포멧 설정에 실패하였습니다.");
			return false;
		}

		if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
		{
			return false;
		}

		if (FAILED(m_pMouse->Acquire()))
		{
			return false;
		}

		SUBSCRIBE_EVENT(Dive::eEventType::BeginFrame, EVENT_HANDLER_PARAM(OnBeginFrame));

		return true;
	}

	void Input::Destroy()
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
	}

	void Input::Update()
	{
		if (!readKeyboard())
			return;

		if (!readMouse())
			return;
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

	void Input::OnBeginFrame(const Event& evnt)
	{
		// event InputBegin
		Update();
		// event InputEnd
	}

	bool Input::readKeyboard()
	{
		memcpy(m_OldKeyStates, m_KeyStates, sizeof(m_KeyStates));
		ZeroMemory(m_KeyStates, sizeof(m_KeyStates));
		
		HRESULT hResult = m_pKeyboard->GetDeviceState(sizeof(m_KeyStates), (LPVOID)&m_KeyStates);
		if (FAILED(hResult))
		{
			ZeroMemory(&m_KeyStates, sizeof(m_KeyStates));

			if ((hResult == DIERR_INPUTLOST) || (hResult == DIERR_NOTACQUIRED))
				m_pKeyboard->Acquire();
			else
				return false;
		}

		return true;
	}
	
	bool Input::readMouse()
	{
		memcpy(m_OldMouseButtons, m_MouseButtons, sizeof(m_MouseButtons));
		ZeroMemory(&m_MouseButtons, sizeof(m_MouseButtons));

		HRESULT hResult = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_MouseState);
		if (FAILED(hResult))
		{
			if ((hResult == DIERR_INPUTLOST) || (hResult == DIERR_NOTACQUIRED))
				m_pMouse->Acquire();
			else
				return false;
		}

		for(int i = 0; i < 8; ++i)
			m_MouseButtons[i] = m_MouseState.rgbButtons[i] ? 1 : 0;

		return true;
	}
}