#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Graphics;
	class Viewport;
	class View;
	class Event;
	class Texture;
	class Texture2D;
	class Shader;

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

		Viewport* GetViewport(unsigned int index);
		void SetViewport(unsigned int index, Viewport* pView);

		Texture* GetScreenBuffer(int width, int height, DXGI_FORMAT format, bool bCubemap);

		void QueueViewportByRenderTarget(Texture* pRenderTarget);

	private:
		void initialize();

		void createRenderTargets();

		// shaders
		// blenders
		// depthstencil states
		// rasterizer states

		void queueViewport(Texture* pRenderTarget, Viewport* pViewport);
		void updateQueuedViewport(unsigned int index);

	private:
		Graphics* m_pGraphics;

		bool m_bInitialized;

		// viewports, queuedViewports, views 이렇게 관리한다.

		std::vector<Viewport*> m_Viewports;
		std::vector<std::pair<Texture*, Viewport*>> m_QueuedViewports;
		std::vector<View*> m_Views;

		// renderTargets를 직접 관리하는 것은 맞았다.
		// 다만 그 과정에서 key를 사용한다는 점이 좀 복잡하다.
		std::unordered_map<unsigned long long, unsigned int> m_ScreenBufferIndices;
		std::unordered_map<unsigned long long, std::vector<Texture*>> m_ScreenBuffers;
	};
}