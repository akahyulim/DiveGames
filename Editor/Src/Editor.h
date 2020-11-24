#pragma once
#include <memory>
#include <vector>
#include <Windows.h>

namespace Dive
{
	class Engine;
	class SystemManager;
}

class IWidget;

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
		if (!m_Widgets.empty())
		{
			for (const auto& widget : m_Widgets)
			{
				if (T* targetWidget = std::dynamic_cast<T*>(widget.get()))
					return targetWidget;
			}
			return nullptr;
		}
	}

	Dive::SystemManager* GetSystemManager() { return m_SystemManager; }

private:

private:
	std::unique_ptr<Dive::Engine> m_Engine;
	Dive::SystemManager* m_SystemManager = nullptr;

	bool m_bInitialized = false;

	std::vector<std::shared_ptr<IWidget>> m_Widgets;
};

