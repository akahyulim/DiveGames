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

		void ComposeUI();

	private:
		Editor* m_Editor = nullptr;
	};
}
