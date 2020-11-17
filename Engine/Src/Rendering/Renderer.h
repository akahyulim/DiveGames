#pragma once
#include "Core/ISystem.h"

namespace Dive
{
	class SystemManager;
	class WindowResizeEvent;
	class RenderDevice;

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
	};
}

