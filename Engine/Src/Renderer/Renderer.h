#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Graphics;
	class View;
	class Event;

	class Renderer : public Object
	{
		DIVE_OBJECT(Renderer, Object)

	public:
		explicit Renderer(Context* pContext);
		~Renderer();

		void Update(float delta);
		void Render();

		void OnRenderUpdate(const Event& e);
		void OnScreenMode(const Event& e);

		std::shared_ptr<View> GetView(unsigned int index);
		void SetView(unsigned int index, std::shared_ptr<View>& view);

	private:
		void initialize();

	private:
		Graphics* m_pGraphics;

		bool m_bInitialized;

		std::vector<std::shared_ptr<View>> m_Views;
	};
}