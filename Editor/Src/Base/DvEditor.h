#pragma once
#include "DiveEngine.h"

namespace Editor
{
	class DvEditor : public Dive::Application
	{
	public:
		DvEditor(Dive::DvContext* pContext);
		~DvEditor();

		void Setup() override;
		void Start() override;
		void Stop() override;

	private:
	private:

	};
}