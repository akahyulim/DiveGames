#pragma once
#include "GraphicsDevice.h"
#include <memory>

namespace Dive
{
	// state, shader, layout, cb, gb 등을 생성 및 관리하는 듯 하다.
	class Renderer
	{
	public:
		static Renderer& GetInstance()
		{
			static Renderer renderer;
			return renderer;
		}

		void Initialize();

		GraphicsDevice* GetDevice() { return m_device.get(); }
		void SetDevice(std::shared_ptr<GraphicsDevice> device);

	private:

	private:
		std::shared_ptr<GraphicsDevice> m_device;
	};
}