#pragma once

namespace Dive
{
	inline constexpr uint16_t MAX_NUM_KEYS = 256;
	inline constexpr uint8_t MAX_NUM_BUTTONS = 4;

	class Input
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void Tick();
		static void ReadKeyboard();
		static void ReadMouse();

		static bool KeyState(int key);
		static bool KeyDown(int key);
		static bool KeyUp(int key);
		static bool KeyPress(int key);

		static bool MouseButtonState(int btn);
		static bool MouseButtonDown(int btn);
		static bool MouseButtonUp(int btn);
		static bool MouseButtonPress(int btn);

		static const DirectX::XMFLOAT2& GetMouseMoveDelta();

		static const DirectX::XMFLOAT2& GetMousePosition();

		static bool GetMouseIsInValidRect();

		static void SetMouseIsInValidRect(bool valid);

	private:
		static IDirectInput8* s_DirectInput;
		static IDirectInputDevice8* s_Keyboard;
		static IDirectInputDevice8* s_Mouse;

		static uint8_t s_KeyStates[MAX_NUM_KEYS];
		static uint8_t s_OldKeyStates[MAX_NUM_KEYS];

		static DIMOUSESTATE s_MouseState;

		static uint8_t s_MouseButtons[MAX_NUM_BUTTONS];
		static uint8_t s_OldMouseButtons[MAX_NUM_BUTTONS];
		static DirectX::XMFLOAT2 s_MousePosition;
		static DirectX::XMFLOAT2 s_OldMousePosition;
		static DirectX::XMFLOAT2 s_MouseDelta;

		static bool s_MouseIsInValidRect;
	};
}
