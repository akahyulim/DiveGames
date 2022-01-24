#pragma once
#include "Src/DiveEngine.h"

// �׷����� namespace�� �� ���ٰ���.
namespace Dive
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
	};
}