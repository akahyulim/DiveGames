#pragma once
#include "Core/System.h"

namespace Dive
{
	class SystemManager;
	class WindowResizeEvent;
	class RenderDevice;
	class DepthStencilState;
	class RasterizerState;
	class BlendState;
	class Texture2D;

	// 굳이 객체들을 구분해 놓을 필요가 있나하는 생각이 든다.
	class Renderer : public System
	{
	public:
		Renderer(const std::shared_ptr<SystemManager>& manager);
		~Renderer();

		bool Initialize() override;
		void Update() override;

		void Present();

		void OnResize(const WindowResizeEvent* evnt);

		const std::shared_ptr<RenderDevice>& GetRenderDevice()	const { return m_renderDevice; }
		const D3D11_VIEWPORT& GetBackbufferViewport()			const { return m_viewport; }

		// Pipeline Pass

	private:

	private:
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

