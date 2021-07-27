#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <array>
#include "Vector2.h"

namespace dive
{
	// Ű�� ��ư�� ��� ����
	enum class eKeyCode
	{
		// keyboard
		Keyboard_F1 = 0,
		Keyboard_F2,
		Keyboard_F3,
		Keyboard_F4,
		Keyboard_F5,
		Keyboard_F6,
		Keyboard_F7,
		Keyboard_F8,
		Keyboard_F9,
		Keyboard_F10,
		Keyboard_F11,
		Keyboard_F12,

		Keyboard_Q,
		Keyboard_W,
		Keyboard_E,
		Keyboard_R,


		// mouse
		Mouse_LeftButton,
		Mouse_MiddleButton,
		Mouse_RightButton,

		// pad
	};

	class Input
	{
	public:
		static Input& GetInstance()
		{
			static Input instance;
			return instance;
		}

		void Initialize(HWND hWnd);
		void Update();
        void ParseMessage(LPARAM lParam);   // ���߿� �̸� �ٲٱ�

		bool GetKey(const eKeyCode key) { return m_Keys[static_cast<unsigned int>(key)]; }
		bool GetKeyDown(const eKeyCode key);
		bool GetKeyUp(const eKeyCode key);
  
		Vector2 GetMousePosition() const { return m_MousePos; }
		void SetMousePosition(const Vector2& position);

		Vector2 GetMousePositionDelta() const { return m_MousePosDelta; }
		float GetMouseWheelDelta() const { return m_MouseWheelDelta; }

	private:

	private:
		HWND m_hWnd = 0;

		bool m_bUpdated = false;

		// keys
		bool m_Keys[256]		= { false };
		bool m_OldKeys[256]	= { false };

		// mouse
		Vector2 m_MousePos = Vector2::Zero;
		Vector2 m_MousePosDelta = Vector2::Zero;
		float m_MouseWheelDelta				= 0.0f;

  };
}