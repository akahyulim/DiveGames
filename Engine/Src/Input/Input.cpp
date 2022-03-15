#include "divepch.h"
#include "Input.h"

// 일단 static으로 event를 등록할 수 있나 확인해야 한다.
// 그리고 WndProc으로부터 event를 어떻게 전달할지 구현해야 한다.
// 스파르탄의 경우 msg별로 event를 만든 것이 아니고
// 매 프레임 WindowData라는 구조체를 채운 후 이벤트로 보낸다.
// 문제는 현재 이벤트를 사용하고 있지 않다는 것이다.
namespace Dive
{
	unsigned int Input::m_MouseStartIndex = static_cast<unsigned int>(eKeyCode::Click_Left);
	unsigned int Input::m_GamePadStartIndex = static_cast<unsigned int>(eKeyCode::DPad_Up);

	// mouse
	DirectX::XMFLOAT2 Input::m_MousePosition = DirectX::XMFLOAT2(0.0f, 0.0f);
	DirectX::XMFLOAT2 Input::m_MouseDelta = DirectX::XMFLOAT2(0.0f, 0.0f);
	float Input::m_MouseWheelDelta = 0.0f;

	// gamepad
	bool Input::m_bGamePadConnected = false;

	void Input::Initialize()
	{
	}

	void Input::Update(float elapsedTime)
	{
		// 패드는 여기서 처리한다.
	}

	// WndProc -> editor -> engine 순으로 호출된다.
	// 구분하는 메시지는 WM_INPUT과 WM_DEVICECHANGE 두 개뿐이다.
	void Input::OnInputMsg()
	{
		// 입력 여부는 WinAPI로 직접 확인한다.
		
		// keyboard
		{
			m_Keys[0] = (::GetKeyState(VK_F1) & 0x8000) != 0;
		}

		// mouse
		{
			m_Keys[m_MouseStartIndex]		= (::GetKeyState(VK_LBUTTON) & 0x8000) != 0;
			m_Keys[m_MouseStartIndex + 1]	= (::GetKeyState(VK_MBUTTON) & 0x8000) != 0;
			m_Keys[m_MouseStartIndex + 2]	= (::GetKeyState(VK_RBUTTON) & 0x8000) != 0;

			// delta 계산을 위해선 WindowData가 필요하다...
		}
	}
	
	bool Input::GetKey(eKeyCode key)
	{
		return m_Keys[static_cast<unsigned int>(key)];
	}
	
	bool Input::KeyDown(eKeyCode key)
	{
		return GetKey(key) && !m_OldKeys[static_cast<unsigned int>(key)];
	}
	
	bool Input::KeyUp(eKeyCode key)
	{
		return !GetKey(key) && m_OldKeys[static_cast<unsigned int>(key)];
	}
}