#pragma once
#include "Dive.h"

namespace LoadStaticModel
{
	class LoadStaticModel : public Dive::DvApplication
	{
	public:
		LoadStaticModel();
		~LoadStaticModel();

		void OnSetup() override;
		void OnStart() override;
		void OnStop() override;

		void HandleUpdate(const Dive::Event& e);

	private:
	private:
		Dive::DvScene* m_pScene;

		Dive::DvGameObject* m_pMainCam;
		Dive::DvGameObject* m_pStaticModel;
	};
}