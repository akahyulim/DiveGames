#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <array>

namespace Dive
{
	// 키와 버튼을 모두 맵핑
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
        void ParseMessage(LPARAM lParam);   // 나중에 이름 바꾸기

		bool GetKey(const eKeyCode key) { return m_keys[static_cast<unsigned int>(key)]; }
		bool GetKeyDown(const eKeyCode key);
		bool GetKeyUp(const eKeyCode key);
  
		const DirectX::XMFLOAT2& GetMousePosition() const { return m_mousePos; }
		void SetMousePosition(const DirectX::XMFLOAT2& position);
		const DirectX::XMFLOAT2& GetMousePosDelta() const { return m_mousePosDelta; }
		float GetMouseWheelDelta() const { return m_mouseWheelDelta; }

	private:

	private:
		HWND m_hWnd = 0;

		bool m_bUpdated = false;

		// keys
		bool m_keys[256] = { false };
		bool m_oldKeys[256] = { false };

		// mouse
		DirectX::XMFLOAT2 m_mousePos		= DirectX::XMFLOAT2(0.0f, 0.0f);
		DirectX::XMFLOAT2 m_mousePosDelta	= DirectX::XMFLOAT2(0.0f, 0.0f);
		float m_mouseWheelDelta				= 0.0f;

  };
}