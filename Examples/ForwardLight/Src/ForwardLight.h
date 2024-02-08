#pragma once
#include "Dive.h"

namespace ForwardLight
{
	class ForwardLight : public Dive::DvApplication
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
		Dive::DvScene* m_pScene;
		Dive::DvGameObject* m_pMainCam;

		Dive::DvGameObject* m_pCube;
		Dive::DvGameObject* m_pTriangle;
		Dive::DvGameObject* m_pSphere;
	};
}