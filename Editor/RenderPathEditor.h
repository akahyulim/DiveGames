#pragma once
#include "Src/Core/DiveEngine.h"

// �׷����� namespace�� �� ���ٰ���.
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
	};
}