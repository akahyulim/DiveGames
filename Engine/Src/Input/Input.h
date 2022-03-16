#pragma once
#include "Events/EventSystem.h"

namespace Dive
{	
	class Input
	{
	public:
		static void Initialize(HWND hWnd);

		static void Update(float elapsedTime);

        static void OnWindowData(const Event& e);

        static bool GetKey(int key);
        static bool KeyDown(int key);
        static bool KeyUp(int key);
        static bool KeyPress(int key);

        static DirectX::XMFLOAT2 GetMousePosition() { return m_MousePosition; }
        static void SetMousePosition(const DirectX::XMFLOAT2& pos);
        static void SetMousePosition(float x, float y);
        static DirectX::XMFLOAT2 GetMouseDelta() { return m_MouseDelta; }
        static float GetMouseWheelDelta() { return m_MouseWheelDelta; }
		
		// game pad

	private:
        static HWND m_WindowHandle;

        static std::array<bool, 256> m_Keys;
        static std::array<bool, 256> m_OldKeys;

        // mouse
        static  DirectX::XMFLOAT2 m_MousePosition;
        static DirectX::XMFLOAT2 m_MouseDelta;
        static float m_MouseWheelDelta;

        // gamepad
        static bool m_bGamePadConnected;

        static bool m_bNewFrame;
        static bool m_bDeviceChange;
	};
}