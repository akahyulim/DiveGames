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

	bool Initialize(HINSTANCE hInstance, HWND hWnd);

	void Update();

private:

private:
	std::unique_ptr<Dive::Engine> m_Engine;

};

