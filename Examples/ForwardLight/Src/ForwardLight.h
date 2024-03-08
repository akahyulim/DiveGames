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
		Dive::GameObject* m_pMainCam;

		Dive::GameObject* m_pCube;
		Dive::GameObject* m_pTriangle;
		Dive::GameObject* m_pSphere;

		Dive::GameObject* m_pDirLightA;
		Dive::GameObject* m_pPointLightA;
		Dive::GameObject* m_pPointLightB;
		Dive::GameObject* m_pPointLightC;
		Dive::GameObject* m_pSpotLightA;
		Dive::GameObject* m_pSpotLightB;
		Dive::GameObject* m_pSpotLightC;
	};
}