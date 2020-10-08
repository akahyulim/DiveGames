#pragma once
#include <memory>
#include <vector>

namespace Dive
{
	class Engine;
}

class Editor
{
public:
	Editor();
	~Editor();

	void Update();

private:

private:
	std::unique_ptr<Dive::Engine> m_engine;

};

