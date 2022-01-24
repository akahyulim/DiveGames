#pragma once
#include "Src/DiveEngine.h"

// 그러고보니 namespace가 좀 에바같다.
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