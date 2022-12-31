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
	class VertexBuffer;
	class ShaderVariation;
	class Material;
	class RenderPath;
	class Batch;
	class BatchQueue;
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

		Material* GetDefaultMaterial() const { return m_pDefaultMaterial; }

		RenderPath* GetDefaultRenderPath() const;
		void SetDefaultRenderPath(RenderPath* pRenderPath);

		Technique* GetDefaultTechnique();
		void SetDefaultTechnique(Technique* pTechnique);
		
		Viewport* GetViewport(uint32_t index);
		void SetViewport(uint32_t index, Viewport* pView);

		Texture* GetScreenBuffer(int32_t width, int32_t height, DXGI_FORMAT format, bool bCubemap);

		void QueueViewportByRenderTarget(Texture* pRenderTarget);

		VertexBuffer* GetInstancingBuffer() { return m_pInstancingBuffer; }

		void SetBatchShaders(Batch& batch, Technique& tech, bool bAllowShadow, const BatchQueue& queue);

	private:
		void initialize();

		void createRenderTargets();

		void createInstancingBuffer();

		// shaders
		// blenders
		// depthstencil states
		// rasterizer states

		void queueViewport(Texture* pRenderTarget, Viewport* pViewport);
		void updateQueuedViewport(uint32_t index);

	private:
		Graphics* m_pGraphics;

		Material* m_pDefaultMaterial;
		RenderPath* m_pDefaultRenderPath;
		Technique* m_pDefaultTechnique;

		bool m_bInitialized;

		// viewports, queuedViewports, views 이렇게 관리한다.

		std::vector<Viewport*> m_Viewports;
		std::vector<std::pair<Texture*, Viewport*>> m_QueuedViewports;
		std::vector<View*> m_Views;

		// renderTargets를 직접 관리하는 것은 맞았다.
		// 다만 그 과정에서 key를 사용한다는 점이 좀 복잡하다.
		std::unordered_map<uint64_t, uint32_t> m_ScreenBufferIndices;
		std::unordered_map<uint64_t, std::vector<Texture*>> m_ScreenBuffers;

		VertexBuffer* m_pInstancingBuffer;
	};
}