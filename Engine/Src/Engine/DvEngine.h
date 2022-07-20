#pragma once
#include "Core/DvObject.h"
#include "Core/Variant.h"

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
		bool Initialize(const VariantMap& parameters);

		void RunFrame();
		void Update();
		void Render();

		bool IsInitialized() const { return m_bInitialized; }
		bool IsExiting() const { return m_bExiting; }

		void Exit();

		void OnExitRequested();

	private:
		void doExit();

	private:
		bool m_bInitialized;
		bool m_bExiting;
	};
}

