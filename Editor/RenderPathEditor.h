#pragma once
#include "DiveEngine.h"

namespace Editor
{
	class RenderPathEditor : public Dive::RenderPath
	{
	public:
		RenderPathEditor();
		~RenderPathEditor();

		//= OVERRIDE ================================
		void PreUpdate() override;
		void Update(float deltaTime) override;
		void Render() const override;
		void Compose() const override;
		//============================================

	private:
	private:
		Dive::Scene* m_pScene;
	};
}