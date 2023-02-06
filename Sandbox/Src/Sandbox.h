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
		//std::shared_ptr<Dive::GameObject> m_pCamera;	// �� �̻��� �� �ִ�. uhro�� �ϳ��� camera ��ü�� �ڽ��� �ξ� camera�� �߰��ߴ�...
		Dive::GameObject* m_pMainCamera;
	};
}