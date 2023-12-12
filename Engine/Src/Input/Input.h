#pragma once

namespace Dive
{
	class Input
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void Update();
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
	};
}