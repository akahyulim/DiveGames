#pragma once
#include "Core/Dive_Object.h"

namespace Dive
{
	class Dive_Context;
	class WindowResizeEvent;
	class RenderDevice;
	class DepthStencilState;
	class RasterizerState;
	class BlendState;
	class Texture2D;

	// 굳이 객체들을 구분해 놓을 필요가 있나하는 생각이 든다.
	class Renderer : public Dive_Object
	{
		NEW_DIVE_OBJECT(Renderer);

	public:
		Renderer(Dive_Context* context);
		~Renderer();

		bool Initialize();
		void Update(size_t eventType);

		void Present();

		void OnResize(const WindowResizeEvent* evnt);

		bool IsInitialized() const { return m_bInitialized; }

		const std::shared_ptr<RenderDevice>& GetRenderDevice()	const { return m_renderDevice; }
		const D3D11_VIEWPORT& GetBackbufferViewport()			const { return m_viewport; }

		// Pipeline Pass

	private:

	private:
		bool m_bInitialized;

		std::shared_ptr<RenderDevice> m_renderDevice;

		D3D11_VIEWPORT m_viewport;

		// DepthStencilStates
		std::shared_ptr<DepthStencilState> m_depthStencilStateEnabled;
		std::shared_ptr<DepthStencilState> m_depthStencilStateDisabled;

		// RasterizerStates
		std::shared_ptr<RasterizerState> m_rasterizerStateCullBackSolid;
		std::shared_ptr<RasterizerState> m_rasterizerStateCullFrontSolid;
		std::shared_ptr<RasterizerState> m_rasterizerStateCullNoneSolid;
		std::shared_ptr<RasterizerState> m_rasterizerStateCullBackWireFrame;
		std::shared_ptr<RasterizerState> m_rasterizerStateCullFrontWireFrame;
		std::shared_ptr<RasterizerState> m_rasterizerStateCullNoneWireFrame;

		// BlendStates
		std::shared_ptr<BlendState> m_blendStateEnable;
		std::shared_ptr<BlendState> m_blendStateDisable;
		std::shared_ptr<BlendState> m_blendStateColorAdd;
	};
}

