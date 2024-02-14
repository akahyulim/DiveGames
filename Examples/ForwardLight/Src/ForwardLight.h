#pragma once
#include "Dive.h"

namespace ForwardLight
{
	class ForwardLight : public Dive::Application
	{
	public:
		ForwardLight();
		~ForwardLight();

		void OnSetup() override;
		void OnStart() override;
		void OnStop() override;

		void HandleUpdate(const Dive::Event& e);

	private:
	private:
		Dive::Scene* m_pScene;
		Dive::GameObject* m_pMainCam;

		Dive::GameObject* m_pCube;
		Dive::GameObject* m_pTriangle;
		Dive::GameObject* m_pSphere;
	};
}