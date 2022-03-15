#include "divepch.h"
#include "Input.h"

// �ϴ� static���� event�� ����� �� �ֳ� Ȯ���ؾ� �Ѵ�.
// �׸��� WndProc���κ��� event�� ��� �������� �����ؾ� �Ѵ�.
// ���ĸ�ź�� ��� msg���� event�� ���� ���� �ƴϰ�
// �� ������ WindowData��� ����ü�� ä�� �� �̺�Ʈ�� ������.
// ������ ���� �̺�Ʈ�� ����ϰ� ���� �ʴٴ� ���̴�.
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
		// �е�� ���⼭ ó���Ѵ�.
	}

	// WndProc -> editor -> engine ������ ȣ��ȴ�.
	// �����ϴ� �޽����� WM_INPUT�� WM_DEVICECHANGE �� �����̴�.
	void Input::OnInputMsg()
	{
		// �Է� ���δ� WinAPI�� ���� Ȯ���Ѵ�.
		
		// keyboard
		{
			m_Keys[0] = (::GetKeyState(VK_F1) & 0x8000) != 0;
		}

		// mouse
		{
			m_Keys[m_MouseStartIndex]		= (::GetKeyState(VK_LBUTTON) & 0x8000) != 0;
			m_Keys[m_MouseStartIndex + 1]	= (::GetKeyState(VK_MBUTTON) & 0x8000) != 0;
			m_Keys[m_MouseStartIndex + 2]	= (::GetKeyState(VK_RBUTTON) & 0x8000) != 0;

			// delta ����� ���ؼ� WindowData�� �ʿ��ϴ�...
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