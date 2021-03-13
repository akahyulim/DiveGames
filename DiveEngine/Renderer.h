#pragma once
#include "GraphicsDevice.h"
#include <memory>

namespace Dive
{
	// state, shader, layout, cb, gb ���� ���� �� �����ϴ� �� �ϴ�.
	class Renderer
	{
	public:
		~Renderer();

		static Renderer& GetInstance()
		{
			static Renderer instance;
			return instance;
		}

		void Initialize();

		// update(constant buffer), draw ���� �����Ǿ� �ִ�.
		// �̷� �̷��� scene�� data�� ������� �� �� �ִ�.
		// ���⿡�� ������ ��ҵ��� RenderPath���� �����Ͽ� �׸��� �����̴�.

		GraphicsDevice* GetDevice() { return m_device.get(); }
		void SetDevice(std::shared_ptr<GraphicsDevice> device);

	private:
		Renderer() = default;

	private:
		std::shared_ptr<GraphicsDevice> m_device;
	};
}