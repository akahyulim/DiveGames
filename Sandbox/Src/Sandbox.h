#pragma once
#include "Dive.h"

using namespace Dive;

class Sandbox : public Application
{
public:
	Sandbox();
	~Sandbox();

	void Setup() override;
	void Start() override;
	void Stop() override;

private:
	bool createTriangle();

private:
	std::shared_ptr<Scene> m_scene;
};


