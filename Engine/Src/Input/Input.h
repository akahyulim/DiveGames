#pragma once

namespace Dive
{
	inline constexpr uint16_t MAX_NUM_KEYS = 256;
	inline constexpr uint8_t MAX_NUM_BUTTONS = 4;

	class Input
	{
	public:
		Input(const Input&) = delete;
		void operator=(const Input&) = delete;

		static Input* GetInstance()
		{
			if (!s_pInstance)
				s_pInstance = new Input;

			return s_pInstance;
		}

		bool Initialize();
		void Shutdown();

		void Update();
		void ReadKeyboard();
		void ReadMouse();

		bool KeyState(int key);
		bool KeyDown(int key);
		bool KeyUp(int key);
		bool KeyPress(int key);

		bool MouseButtonState(int btn);
		bool MouseButtonDown(int btn);
		bool MouseButtonUp(int btn);
		bool MouseButtonPress(int btn);

		const DirectX::XMFLOAT2& GetMouseMoveDelta();

		const DirectX::XMFLOAT2& GetMousePosition();

		bool GetMouseIsInValidRect();
		void SetMouseIsInValidRect(bool valid);

	private:
		Input();
		~Input();

	private:
		static Input* s_pInstance;

		IDirectInput8* m_pDirectInput;
		IDirectInputDevice8* m_pKeyboard;
		IDirectInputDevice8* m_pMouse;

		uint8_t m_KeyStates[MAX_NUM_KEYS];
		uint8_t m_OldKeyStates[MAX_NUM_KEYS];

		DIMOUSESTATE m_MouseState;

		uint8_t m_MouseButtons[MAX_NUM_BUTTONS];
		uint8_t m_OldMouseButtons[MAX_NUM_BUTTONS];
		DirectX::XMFLOAT2 m_MousePosition;
		DirectX::XMFLOAT2 m_OldMousePosition;
		DirectX::XMFLOAT2 m_MouseDelta;

		bool m_MouseIsInValidRect;
	};

	Input* GetInput();
}