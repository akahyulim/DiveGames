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

	void OnUpdate(const Dive::Event& e);
	void OnPostUpdate(const Dive::Event& e);
	void OnWindowEvent(const Dive::Event& e);

private:

private:
};