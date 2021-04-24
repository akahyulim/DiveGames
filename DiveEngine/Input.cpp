#include "Input.h"
#include "Log.h"

namespace Dive
{
    void Input::Initialize(HWND hWnd)
    {
        m_hWnd = hWnd;

        // mouse
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01;
        rid.usUsage     = 0x02;
        rid.dwFlags     = RIDEV_INPUTSINK;
        rid.hwndTarget  = m_hWnd;

        if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
        {
            CORE_ERROR("");
        }
    }

    void Input::Update()
    {
        memcpy(m_oldKeys, m_keys, sizeof(m_keys));
        memset(m_keys, false, sizeof(m_keys));

        // keyboard
        {
            m_keys[(size_t)eKeyCode::Keyboard_F1] = (::GetKeyState(VK_F1) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F2] = (::GetKeyState(VK_F2) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F3] = (::GetKeyState(VK_F3) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F4] = (::GetKeyState(VK_F4) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F5] = (::GetKeyState(VK_F5) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F6] = (::GetKeyState(VK_F6) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F7] = (::GetKeyState(VK_F7) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F8] = (::GetKeyState(VK_F8) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F9] = (::GetKeyState(VK_F9) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F10] = (::GetKeyState(VK_F10) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F11] = (::GetKeyState(VK_F11) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_F12] = (::GetKeyState(VK_F12) & 0x8000) != 0;

            m_keys[(size_t)eKeyCode::Keyboard_Q] = (::GetKeyState('Q') & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_W] = (::GetKeyState('W') & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_E] = (::GetKeyState('E') & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Keyboard_R] = (::GetKeyState('R') & 0x8000) != 0;
        }

        // mouse
        {
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

            // button
            m_keys[(size_t)eKeyCode::Mouse_LeftButton]      = (::GetKeyState(VK_LBUTTON) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Mouse_MiddleButton]    = (::GetKeyState(VK_MBUTTON) & 0x8000) != 0;
            m_keys[(size_t)eKeyCode::Mouse_RightButton]     = (::GetKeyState(VK_RBUTTON) & 0x8000) != 0;
        }

        m_bUpdated = true;
    }

    // WM_INPUT의 lparam
    void Input::ParseMessage(LPARAM lParam)
    {
        if (m_bUpdated)
        {
            // 이걸 왜 초기화할까?
            // 아 프레임 단위로 안하면 결국 값은 계산 커지니깐?
            m_mousePosDelta = DirectX::XMFLOAT2(0.0f, 0.0f);
        }

        unsigned int cbSize = 0;
        auto result = GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &cbSize, sizeof(RAWINPUTHEADER));
        assert(result == 0);

        void* pData = new BYTE[cbSize];
        result = GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, pData, &cbSize, sizeof(RAWINPUTHEADER));
        if (result != cbSize)
        {
            CORE_ERROR("");
        }

        auto pRaw = static_cast<RAWINPUT*>(pData);
        if (pRaw->header.dwType == RIM_TYPEMOUSE)
        {
            const RAWMOUSE& mouse = pRaw->data.mouse;
            
            if (mouse.usFlags == MOUSE_MOVE_RELATIVE)
            {
                if (std::abs(mouse.lLastX) < 30000)
                {
                    m_mousePosDelta.x += static_cast<float>(mouse.lLastX);
                }
                if (std::abs(mouse.lLastY) < 30000)
                {
                    m_mousePosDelta.y += static_cast<float>(mouse.lLastY);
                }

                if (mouse.usButtonFlags == RI_MOUSE_WHEEL)
                {
                    m_mouseWheelDelta += static_cast<float>(static_cast<SHORT>(mouse.usButtonData)) / static_cast<float>(WHEEL_DELTA);
                }
            }
        }

        delete[] pData;
        pData = nullptr;

        m_bUpdated = false;
    }

    bool Input::GetKeyDown(const eKeyCode key)
    {
        return (m_oldKeys[static_cast<unsigned int>(key)] && m_keys[static_cast<unsigned int>(key)]);
    }
    
    bool Input::GetKeyUp(const eKeyCode key)
    {
        return (m_oldKeys[static_cast<unsigned int>(key)] && !m_keys[static_cast<unsigned int>(key)]);
    }

    void Input::SetMousePosition(const DirectX::XMFLOAT2& position)
    {
        if (m_hWnd == ::GetActiveWindow())
        {
            POINT pt = POINT{ static_cast<LONG>(position.x), static_cast<LONG>(position.y) };
            if (::ClientToScreen(m_hWnd, &pt))
            {
                ::SetCursorPos(static_cast<int>(pt.x), static_cast<int>(pt.y));
            }
        }
    }
}