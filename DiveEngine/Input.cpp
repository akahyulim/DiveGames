#include "Input.h"
#include "Log.h"

namespace Dive
{
    // 패드, 조이스틱 지원 여부를 전달 받은 후 등록해야 한다.
    void Input::Initialize(HWND hWnd)
    {
        m_hWnd = hWnd;

        // mouse
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01;
        rid.usUsage = 0x02;
        rid.dwFlags = RIDEV_INPUTSINK;
        rid.hwndTarget = m_hWnd;

        if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
        {
            assert(0);
        }
    }

    void Input::Update()
    {
        // x input을 여기서 처리해야할 이유가 있나?

        // 혼란스럽다. 여기에선 되는 이유가 뭐지?
        if (::GetKeyState(VK_F1) & 0x8000) //(GetKey(eKeyCode::Keyboard_F1))
        {
            MessageBox(m_hWnd, L"You Press F1", L"Detected Key Press", MB_OK);
        }

        m_bUpdated = true;
    }

    // WM_INPUT의 lparam
    void Input::ParseMessage(LPARAM lParam)
    {
        if (m_bUpdated)
        {
            memcpy(m_oldKeys, m_keys, sizeof(m_keys));
            // 이걸 왜 초기화할까?
            // 아 프레임 단위로 안하면 결국 값은 계산 커지니깐?
            m_mousePosDelta = DirectX::XMFLOAT2(0.0f, 0.0f);
        }

        // keyboard
        {
            m_keys[(size_t)eKeyCode::Keyboard_F1]   = (::GetKeyState(VK_F1) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F2]   = (::GetKeyState(VK_F2) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F3]   = (::GetKeyState(VK_F3) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F4]   = (::GetKeyState(VK_F4) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F5]   = (::GetKeyState(VK_F5) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F6]   = (::GetKeyState(VK_F6) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F7]   = (::GetKeyState(VK_F7) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F8]   = (::GetKeyState(VK_F8) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F9]   = (::GetKeyState(VK_F9) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F10]  = (::GetKeyState(VK_F10) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F11]  = (::GetKeyState(VK_F11) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F12]  = (::GetKeyState(VK_F12) & 0x8000) != 0;

            m_keys[(size_t)eKeyCode::Keyboard_Q]    = (::GetKeyState('Q') & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_W]    = (::GetKeyState('W') & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_E]    = (::GetKeyState('E') & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_R]    = (::GetKeyState('R') & 0x8000) != 0;
        }

        // mouse
        {
            // 결국 mouse를 등록했기 때문이 이 함수가 작동하는 거다.

            // button
            m_keys[(size_t)eKeyCode::Mouse_LeftButton]      = (::GetKeyState(VK_LBUTTON) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Mouse_MiddleButton]    = (::GetKeyState(VK_MBUTTON) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Mouse_RightButton]     = (::GetKeyState(VK_RBUTTON) & 0x8000) != 0;

            // position
            if (m_hWnd == ::GetActiveWindow())
            {
                POINT pt;
                if (::GetCursorPos(&pt))
                {
                    ::ScreenToClient(m_hWnd, &pt);
                    m_mousePos = DirectX::XMFLOAT2(static_cast<float>(pt.x), static_cast<float>(pt.y));
                }
            }

            // pos delta 확인
            RAWINPUT data;
            ZeroMemory(&data, sizeof(RAWINPUT));
            UINT cbSize = sizeof(RAWINPUT);

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &data, &cbSize, sizeof(RAWINPUTHEADER));
            
            if (data.header.dwType == RIM_TYPEMOUSE)
            {
                // 누적을 해야하나?
                m_mousePosDelta.x = static_cast<float>(data.data.mouse.lLastX);
                m_mousePosDelta.y = static_cast<float>(data.data.mouse.lLastY);

                if (data.data.mouse.usButtonFlags == RI_MOUSE_WHEEL)
                {
                    m_mouseWheelDelta = static_cast<float>(static_cast<SHORT>(data.data.mouse.usButtonData)) / static_cast<float>(WHEEL_DELTA);
                }
            }
        }

        m_bUpdated = false;
    }
}