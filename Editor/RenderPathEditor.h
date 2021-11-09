#pragma once
#include "Src/Core/DiveEngine.h"

// �׷����� namespace�� �� ���ٰ���.
namespace DiveEditor
{
	class RenderPathEditor : public DiveEngine::RenderPath
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