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

		const std::shared_ptr<RenderDevice>& GetRenderDevice()	const { return m_RenderDevice; }
		const D3D11_VIEWPORT& GetBackbufferViewport()			const { return m_Viewport; }

		// Pipeline Pass

	private:

	private:
		std::shared_ptr<RenderDevice> m_RenderDevice;

		D3D11_VIEWPORT m_Viewport;

		// DepthStencilStates
		std::shared_ptr<DepthStencilState> m_DepthStencilStateEnabled;
		std::shared_ptr<DepthStencilState> m_DepthStencilStateDisabled;

		// RasterizerStates
		std::shared_ptr<RasterizerState> m_RasterizerStateCullBackSolid;
		std::shared_ptr<RasterizerState> m_RasterizerStateCullFrontSolid;
		std::shared_ptr<RasterizerState> m_RasterizerStateCullNoneSolid;
		std::shared_ptr<RasterizerState> m_RasterizerStateCullBackWireFrame;
		std::shared_ptr<RasterizerState> m_RasterizerStateCullFrontWireFrame;
		std::shared_ptr<RasterizerState> m_RasterizerStateCullNoneWireFrame;

		// BlendStates
		std::shared_ptr<BlendState> m_BlendStateEnable;
		std::shared_ptr<BlendState> m_BlendStateDisable;
		std::shared_ptr<BlendState> m_BlendStateColorAdd;
	};
}

