#pragma once
#include "Dive.h"

namespace Sandbox
{
	class Sandbox : public Dive::DvApplication
	{
	public:
		Sandbox();
		~Sandbox();

		void OnSetup() override;
		void OnStart() override;
		void OnStop() override;

	private:
	private:
		Dive::DvScene* m_pScene;
		Dive::DvGameObject* m_pMainCam;
	};
}