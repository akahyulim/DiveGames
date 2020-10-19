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

		// ���� ��ü ������ ������?
		RenderDevice* GetRenderDevice() const { return m_render_device.get(); }

	private:
	private:
		std::shared_ptr<RenderDevice> m_render_device;
	};
}

