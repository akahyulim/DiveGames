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

		// viewports, queuedViewports, views �̷��� �����Ѵ�.

		std::vector<Viewport*> m_Viewports;
		std::vector<std::pair<Texture*, Viewport*>> m_QueuedViewports;
		std::vector<View*> m_Views;

		// renderTargets�� ���� �����ϴ� ���� �¾Ҵ�.
		// �ٸ� �� �������� key�� ����Ѵٴ� ���� �� �����ϴ�.
		std::unordered_map<unsigned long long, unsigned int> m_ScreenBufferIndices;
		std::unordered_map<unsigned long long, std::vector<Texture*>> m_ScreenBuffers;
	};
}