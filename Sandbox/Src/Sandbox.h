#pragma once
#include "DiveEngine.h"

namespace Sandbox
{
	class Sandbox : public Dive::Application
	{
	public:
		Sandbox(Dive::DvContext* pContext);
		~Sandbox();

		void Setup() override;
		void Start() override;
		void Stop() override;

	private:
	private:
		//std::shared_ptr<Dive::Scene> m_pScene;
		//std::shared_ptr<Dive::GameObject> m_pCamera;	// �� �̻��� �� �ִ�. uhro�� �ϳ��� camera ��ü�� �ڽ��� �ξ� camera�� �߰��ߴ�...
	};
}