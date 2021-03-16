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
			static Renderer instance;
			return instance;
		}

		void Initialize();

		// update(constant buffer), draw 등이 구현되어 있다.
		// 이로 미루어보아 scene의 data를 사용함을 알 수 있다.
		// 여기에서 구현된 요소들을 RenderPath에서 구성하여 그리는 형태이다.

		GraphicsDevice* GetDevice() { return m_device.get(); }
		void SetDevice(std::shared_ptr<GraphicsDevice> device);

		// 각종 리소스 생성

	private:
		Renderer() = default;
		~Renderer();

	private:
		std::shared_ptr<GraphicsDevice> m_device;

		// 생성 리소스 관리
	};
}