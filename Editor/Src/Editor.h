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
	Editor();
	~Editor();

	bool Initialize(HWND hWnd, int width, int height, bool vsync, bool windowed);

	void Update();

private:

private:
	std::unique_ptr<Dive::Engine> m_Engine;

};

