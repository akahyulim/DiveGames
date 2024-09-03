#pragma once
#include "Dive.h"

namespace Sandbox
{
	enum eSceneType
	{
		Base,
		Car,
		Sponza,
		Field,
		Texture
	};

	class Sandbox : public Dive::Application
	{
	public:
		Sandbox();
		~Sandbox();

		void OnSetup() override;
		void OnStart() override;
		void OnStop() override;

		void HandleUpdate(const Dive::Event& e);

	private:
		void createBaseScene();
		void createCarScene();
		void createSponzaScene();
		void createFieldScene();
		void createTextureScene();

	private:
		Dive::Graphics* m_pGraphics;
		Dive::Input* m_pInput;

		Dive::GameObject* m_pMainCam;
		DirectX::XMFLOAT3 m_CamDefaultPos;

		Dive::GameObject* m_pDummy;

		Dive::GameObject* m_pCube;
		Dive::GameObject* m_pTriangle;
		Dive::GameObject* m_pSphere;
		Dive::GameObject* m_pCar;

		Dive::GameObject* m_pDirLightA;
		Dive::GameObject* m_pPointLightA;
		Dive::GameObject* m_pPointLightB;
		Dive::GameObject* m_pPointLightC;
		Dive::GameObject* m_pSpotLightA;
		Dive::GameObject* m_pSpotLightB;
		Dive::GameObject* m_pSpotLightC;

		Dive::GameObject* m_pFlashLight;

		eSceneType m_SceneType;
	};
}