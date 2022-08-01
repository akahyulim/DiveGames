#pragma once
#include "Core/DvObject.h"

namespace Dive
{
	class DvContext;
	class DvGraphics;

	class DvRenderer : public DvObject
	{
		DIVE_OBJECT(DvRenderer, DvObject)

	public:
		explicit DvRenderer(DvContext* pContext);
		~DvRenderer();

		void Update(float delta);
		void Render();

		void OnRenderUpdate(const Variant& data);

	private:
		void initialize();

	private:
		std::weak_ptr<DvGraphics> m_pGraphics;

		bool m_bInitialized;
	};
}