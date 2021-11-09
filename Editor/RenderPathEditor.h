#pragma once
#include "Src/Core/DiveEngine.h"

// 그러고보니 namespace가 좀 에바같다.
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