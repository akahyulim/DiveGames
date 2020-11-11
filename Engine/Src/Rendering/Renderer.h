#pragma once
#include "Core/ISystem.h"

namespace Dive
{
	class SystemManager;
	class RenderDevice;
	class WindowResizeEvent;
	class SwapChain;

	class Renderer : public ISystem
	{
	public:
		Renderer(SystemManager* manager);
		~Renderer();

		bool Initialize() override;
		void Update() override;

		void Present();

		// 개별 객체 전달이 나을까?
		RenderDevice* GetRenderDevice() const { return m_RenderDevice.get(); }

		void OnResize(const WindowResizeEvent* evnt);

	private:
	private:
		std::unique_ptr<RenderDevice> m_RenderDevice;
		std::unique_ptr<SwapChain> m_SwapChain;
	};
}

