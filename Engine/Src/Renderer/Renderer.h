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
	class ShaderVariation;
	class RenderPath;
	class Technique;

	class Renderer : public Object
	{
		DIVE_OBJECT(Renderer, Object)

	public:
		explicit Renderer(Context* pContext);
		~Renderer() override;

		void Update(float delta);
		void Render();

		void OnRenderUpdate(const Event& e);
		void OnScreenMode(const Event& e);

		void SetDefaultRenderPath(RenderPath* pRenderPath);
		RenderPath* GetDefaultRenderPath() const;

		void SetDefaultTechnique(Technique* pTechnique);
		Technique* GetDefaultTechnique() const;

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
		Graphics* m_pGraphics = nullptr;
		RenderPath* m_pDefaultRenderPath = nullptr;
		Technique* m_pDefaultTechnique = nullptr;

		bool m_bInitialized = false;

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