#pragma once
#include "Core/DvObject.h"

namespace Dive
{
	class DvContext;

	class DvGraphics : public DvObject
	{ 
		DIVE_OBJECT(DvGraphics, DvObject)

	public:
		DvGraphics(DvContext* pContext);
		~DvGraphics();


		bool IsInitialized();

		bool BeginFrame();
		void EndFrame();

	private:
	private:
	};
}