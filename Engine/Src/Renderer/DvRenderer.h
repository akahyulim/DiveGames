#pragma once
#include "Core/DvObject.h"

namespace Dive
{
	class DvContext;
	class DvGraphics;
	class DvView;

	class DvRenderer : public DvObject
	{
		DIVE_OBJECT(DvRenderer, DvObject)

	public:
		explicit DvRenderer(DvContext* pContext);
		~DvRenderer();

		void Update(float delta);
		void Render();

		void OnRenderUpdate(const Variant& data);

		std::shared_ptr<DvView> GetView(unsigned int index);
		void SetView(unsigned int index, std::shared_ptr<DvView>& view);

		void OnScreenMode(const Variant& var);

	private:
		void initialize();

	private:
		DvGraphics* m_pGraphics;

		bool m_bInitialized;

		std::vector<std::shared_ptr<DvView>> m_Views;
	};
}