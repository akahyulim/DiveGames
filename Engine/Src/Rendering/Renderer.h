#pragma once
#include "Core/ISystem.h"

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
	class Renderer : public ISystem
	{
	public:
		Renderer(SystemManager* manager);
		~Renderer();

		bool Initialize() override;
		void Update() override;

		void Present();

		void OnResize(const WindowResizeEvent* evnt);

		RenderDevice* GetRenderDevice() const { return m_RenderDevice; }

	private:

	private:
		RenderDevice* m_RenderDevice = nullptr;

		// DepthStencilStates
		DepthStencilState* m_DepthStencilStateEnabled	= nullptr;
		DepthStencilState* m_DepthStencilStateDisabled	= nullptr;

		// RasterizerStates
		RasterizerState* m_RasterizerStateCullBackSolid			= nullptr;
		RasterizerState* m_RasterizerStateCullFrontSolid		= nullptr;
		RasterizerState* m_RasterizerStateCullNoneSolid			= nullptr;
		RasterizerState* m_RasterizerStateCullBackWireFrame		= nullptr;
		RasterizerState* m_RasterizerStateCullFrontWireFrame	= nullptr;
		RasterizerState* m_RasterizerStateCullNoneWireFrame		= nullptr;

		// BlendStates
		BlendState* m_BlendStateEnable		= nullptr;
		BlendState* m_BlendStateDisable		= nullptr;
		BlendState* m_BlendStateColorAdd	= nullptr;
	};
}

