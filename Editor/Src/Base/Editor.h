#pragma once
#include <string>

namespace Dive
{
	class Engine;
	struct WindowData;

	// 사실상 WindowData와 겹친다...?
	struct EditorData
	{
		std::string Title	= "DiveEditor";
		unsigned int Width	= 1280;
		unsigned int Height = 780;
		bool Maximize		= false;
	};

	class Editor
	{
	public:
		Editor();
		~Editor();

		void Tick();

		void OnWindowMessage(WindowData& data);

		const EditorData& GetData() const { return m_Data; }

	private:

	private:
		// editor
		EditorData m_Data;
		bool m_bInitialized = false;

		// engine
		Engine* m_pEngine = nullptr;

	};
}