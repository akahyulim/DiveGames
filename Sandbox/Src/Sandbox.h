#pragma once
#include "DiveEngine.h"

class Sandbox : public Dive::Application
{
public:
	Sandbox(Dive::DvContext* pContext);
	~Sandbox();

	void Setup() override;
	void Start() override;

private:
private:

};
