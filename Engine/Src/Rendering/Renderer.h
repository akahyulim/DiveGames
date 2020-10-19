#pragma once
#include "Core/ISystem.h"

namespace Dive
{
	class SystemManager;
	class RenderDevice;

	class Renderer : public ISystem
	{
	public:
		Renderer(SystemManager* manager);
		~Renderer();

		bool Initialize() override;
		void Update() override;

		void Present();

		// 개별 객체 전달이 나을까?
		RenderDevice* GetRenderDevice() const { return m_render_device.get(); }

	private:
	private:
		std::shared_ptr<RenderDevice> m_render_device;
	};
}

