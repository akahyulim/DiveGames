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

	private:
		Dive::Model* GetModel(const std::string& name);
		Dive::Material* GetMaterial(const std::string& name);

	private:
		Dive::Scene* m_pScene;
		//std::shared_ptr<Dive::GameObject> m_pCamera;	// �� �̻��� �� �ִ�. uhro�� �ϳ��� camera ��ü�� �ڽ��� �ξ� camera�� �߰��ߴ�...
	};
}