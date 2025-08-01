#pragma once
#include <memory>

namespace Dive
{
	class Editor;
	class World;

	class MenuBar
	{
	public:
		MenuBar(Editor* editor);
		~MenuBar() = default;

		void Draw();

	private:
		void newWorld();

	private:
		Editor* m_editor = nullptr;
	};
}
