#pragma once
#include "EventDispatcher.h"

namespace Dive
{
	inline constexpr uint16_t MAX_NUM_KEYS = 256;
	inline constexpr uint8_t MAX_NUM_BUTTONS = 4;

	class Input
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void OnBeginFrame();

		static void Update();
		static bool ReadKeyboard();
		static bool ReadMouse();
		static void ProcessInput();

		static bool KeyState(int key);
		static bool KeyDown(int key);
		static bool KeyUp(int key);
		static bool KeyPress(int key);

		static bool MouseButtonState(int btn);
		static bool MouseButtonDown(int btn);
		static bool MouseButtonUp(int btn);
		static bool MouseButtonPress(int btn);

		static DirectX::XMFLOAT2 GetMouseMoveDelta();
		static int GetMouseWheelDelta();

	private:
		static Microsoft::WRL::ComPtr<IDirectInput8> s_directInput;
		static Microsoft::WRL::ComPtr<IDirectInputDevice8> s_keyboard;
		static Microsoft::WRL::ComPtr<IDirectInputDevice8> s_mouse;

		static uint8_t s_keyStates[MAX_NUM_KEYS];
		static uint8_t s_oldKeyStates[MAX_NUM_KEYS];

		static DIMOUSESTATE s_mouseState;
		static uint8_t s_mouseButtons[MAX_NUM_BUTTONS];
		static uint8_t s_oldMouseButtons[MAX_NUM_BUTTONS];
	};
}
