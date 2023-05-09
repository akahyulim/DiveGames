#pragma once

namespace Dive
{
	class Input
	{
	public:
		static bool Initialize(HINSTANCE hInstance, HWND hWnd);
		static void Shutdown();

		static void Update(float deltaTime);

		static bool KeyState(int key);
		static bool KeyDown(int key);
		static bool KeyUp(int key);
		static bool KeyPress(int key);

		static bool MouseButtonState(int btn);
		static bool MouseButtonDown(int btn);
		static bool MouseButtonUp(int btn);
		static bool MouseButtonPress(int btn);

		static bool IsInitialized();

	private:
		static bool readKeyboard();
		static bool readMouse();
	};
}