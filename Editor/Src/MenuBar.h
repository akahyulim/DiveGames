#pragma once

namespace Dive
{
	class Editor;
	class Camera;

	class MenuBar
	{
	public:
		static void Initialize(Editor* editor);
		static void Draw();

		static Camera* GetEditorCamera() { return s_EditorCamera; }

	private:
		static Editor* s_Editor;
		static Camera* s_EditorCamera;
	};
}