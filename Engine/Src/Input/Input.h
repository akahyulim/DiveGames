#pragma once

namespace Dive
{	
	enum class eKeyCode
	{
        // Keyboard
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15,/*Function*/
        Alpha0, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,/*Numbers*/
        Keypad0, Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,/*Numpad*/
        Q, W, E, R, T, Y, U, I, O, P,/*Letters*/
        A, S, D, F, G, H, J, K, L,
        Z, X, C, V, B, N, M,
        Esc,/*Controls*/
        Tab,
        Shift_Left, Shift_Right,
        Ctrl_Left, Ctrl_Right,
        Alt_Left, Alt_Right,
        Space,
        CapsLock,
        Backspace,
        Enter,
        Delete,
        Arrow_Left, Arrow_Right, Arrow_Up, Arrow_Down,
        Page_Up, Page_Down,
        Home,
        End,
        Insert,

        // Mouse
        Click_Left,
        Click_Middle,
        Click_Right,

        // Gamepad
        DPad_Up,
        DPad_Down,
        DPad_Left,
        DPad_Right,
        Button_A,
        Button_B,
        Button_X,
        Button_Y,
        Start,
        Back,
        Left_Thumb,
        Right_Thumb,
        Left_Shoulder,
        Right_Shoulder
	};

	class Input
	{
	public:
		static void Initialize();

		static void Update(float elapsedTime);

        static void OnInputMsg();

        static bool GetKey(eKeyCode key);
        static bool KeyDown(eKeyCode key);
        static bool KeyUp(eKeyCode key);

		// get / set mouse position
		// mouse wheel
		// game pad

	private:
        static std::array<bool, 99> m_Keys;
        static std::array<bool, 99> m_OldKeys;
        static unsigned int m_MouseStartIndex;
        static unsigned int m_GamePadStartIndex;

        // mouse
        static  DirectX::XMFLOAT2 m_MousePosition;
        static DirectX::XMFLOAT2 m_MouseDelta;
        static float m_MouseWheelDelta;

        // gamepad
        static bool m_bGamePadConnected;
	};
}