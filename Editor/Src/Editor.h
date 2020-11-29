#pragma once
#include <memory>
#include <vector>
#include <Windows.h>

namespace Dive
{
	class Engine;
	class SystemManager;
}

class Widget;

class Editor
{
public:
	Editor(HINSTANCE hInstance, HWND hWnd, int width, int height, bool windowed);
	~Editor();

	void Update();

	void OnResize(int width, int height);

	bool IsInitialized() const { return m_bInitialized; }

	template<class T>
	T* GetWidget()
	{
		if (!m_widgets.empty())
		{
			for (const auto& widget : m_widgets)
			{
				if (T* targetWidget = std::dynamic_cast<T*>(widget.get()))
					return targetWidget;
			}
			return nullptr;
		}
	}

	const std::shared_ptr<Dive::SystemManager>& GetSystemManager() const { return m_systemManager; }

private:

private:
	std::unique_ptr<Dive::Engine> m_engine;
	std::shared_ptr<Dive::SystemManager> m_systemManager;

	bool m_bInitialized = false;

	std::vector<std::shared_ptr<Widget>> m_widgets;
};

