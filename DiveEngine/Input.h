#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <array>

namespace dive
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

		bool GetKey(const eKeyCode key) { return mKeys[static_cast<unsigned int>(key)]; }
		bool GetKeyDown(const eKeyCode key);
		bool GetKeyUp(const eKeyCode key);
  
		DirectX::XMFLOAT2 GetMousePosition() const { return mMousePos; }
		void SetMousePosition(const DirectX::XMFLOAT2& position);
		
		DirectX::XMFLOAT2 GetMousePosDelta() const { return mMousePosDelta; }
		float GetMouseWheelDelta() const { return mMouseWheelDelta; }

	private:

	private:
		HWND mWindowHandle = 0;

		bool mbUpdated = false;

		// keys
		bool mKeys[256]		= { false };
		bool mOldKeys[256]	= { false };

		// mouse
		DirectX::XMFLOAT2 mMousePos			= DirectX::XMFLOAT2(0.0f, 0.0f);
		DirectX::XMFLOAT2 mMousePosDelta	= DirectX::XMFLOAT2(0.0f, 0.0f);
		float mMouseWheelDelta				= 0.0f;

  };
}