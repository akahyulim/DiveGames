#pragma once

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
		static void Initialize();

		// event... static으로 되나...?

		static void Update(float elapsedTime);

		// key down
		// key up
		// get key
		// get / set mouse position
		// mouse wheel
		// game pad

	private:

	};
}