#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class WindowResizeEvent;
	class RenderDevice;
	class DepthStencilState;
	class RasterizerState;
	class BlendState;
	class Texture2D;

	// ���� ��ü���� ������ ���� �ʿ䰡 �ֳ��ϴ� ������ ���.
	class Renderer : public Object
	{
		DIVE_OBJECT(Renderer, Object);

	public:
		Renderer(Context* context);
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

