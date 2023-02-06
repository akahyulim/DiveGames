#pragma once
#include "DiveEngine.h"

namespace Sandbox
{
	class Sandbox : public Dive::Application
	{
	public:
		explicit Sandbox(Dive::Context* pContext);
		~Sandbox() override;

		void Setup() override;
		void Start() override;
		void Stop() override;

		void OnUpdate(const Dive::Event& evnt);

	private:
		void createScene();
		Dive::Model* getModel(const std::string& name);
		Dive::Technique* getTechnique(const std::string& name);

	private:
		Dive::Scene* m_pScene;
		//std::shared_ptr<Dive::GameObject> m_pCamera;	// 둘 이상일 수 있다. uhro는 하나의 camera 객체에 자식을 두어 camera를 추가했다...
		Dive::GameObject* m_pMainCamera;
	};
}