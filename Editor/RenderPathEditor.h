#pragma once
#include "DiveEngine.h"

namespace Editor
{
	class RenderPathEditor : public dive::RenderPath
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
		dive::Scene* m_pScene;
	};
}