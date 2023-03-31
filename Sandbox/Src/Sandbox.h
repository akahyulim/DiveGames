#pragma once
#include "Dive.h"

class Sandbox : public Dive::Application
{
public:
	Sandbox();
	~Sandbox() override;

	void Setup() override;
	void Start() override;
	void Stop() override;

	void OnUpdate(const Dive::Event& evnt);

private:

private:
};