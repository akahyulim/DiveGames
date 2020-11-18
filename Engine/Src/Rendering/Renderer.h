#pragma once
#include "Core/ISystem.h"

namespace Dive
{
	class SystemManager;
	class WindowResizeEvent;
	class RenderDevice;

	// ���� ��ü���� ������ ���� �ʿ䰡 �ֳ��ϴ� ������ ���.
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

