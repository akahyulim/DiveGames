#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Event;

	// 일단 dinput으로 구현하고 추후 xinput도...
	class Input : public Object
	{
		DIVE_OBJECT(Input, Object)

	public:
		explicit Input(Context* pContext);
		~Input() override;

		bool Initialize(HINSTANCE hInstance, HWND hWnd, int width, int height);
		void Destroy();

		void Update();

		bool KeyState(int key);
		bool KeyDown(int key);
		bool KeyUp(int key);
		bool KeyPress(int key);

		bool MouseButtonState(int btn);
		bool MouseButtonDown(int btn);
		bool MouseButtonUp(int btn);
		bool MouseButtonPress(int btn);

		void OnBeginFrame(const Event& evnt);

	private:
		bool readKeyboard();
		bool readMouse();

	private:
		HWND m_WindowHandle;

		IDirectInput8* m_pDirectInput;
		IDirectInputDevice8* m_pKeyboard;
		IDirectInputDevice8* m_pMouse;

		DIMOUSESTATE m_MouseState;

		uint8_t m_KeyStates[256];
		uint8_t m_OldKeyStates[256];

		uint8_t m_MouseButtons[4];
		uint8_t m_OldMouseButtons[4];

		int m_ScreenWidth;
		int m_ScreenHeight;
	};
}