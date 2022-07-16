#pragma once
#include "Core/DvObject.h"

namespace Dive
{
	class DvContext;

	class DvEngine : public DvObject
	{
		DIVE_OBJECT(DvEngine, DvObject)

	public:
		explicit DvEngine(DvContext* pContext);
		~DvEngine();

		// Application���κ��� �Ű������� �޾ƾ� �Ѵ�.
		bool Initialize();

		void RunFrame();

		bool IsInitialized() const { return m_bInitialized; }
		bool IsExiting() const { return m_bExiting; }

	private:
		void update();
		void render();

	private:
		bool m_bInitialized;
		bool m_bExiting;
	};
}

