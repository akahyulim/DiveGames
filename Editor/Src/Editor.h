#pragma once
#include <memory>
#include <vector>
#include <Windows.h>

namespace Dive
{
	class Engine;
}

class Editor
{
public:
	Editor(HINSTANCE hInstance, HWND hWnd, int width, int height, bool windowed);
	~Editor();

	void Update();

	void OnResize(int width, int height);

private:

private:
	std::unique_ptr<Dive::Engine> m_Engine;

};

