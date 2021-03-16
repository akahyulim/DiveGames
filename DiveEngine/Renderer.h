#pragma once
#include "GraphicsDevice.h"
#include <memory>

namespace Dive
{
	// state, shader, layout, cb, gb ���� ���� �� �����ϴ� �� �ϴ�.
	class Renderer
	{
	public:
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

		// ���� ���ҽ� ����

	private:
		Renderer() = default;
		~Renderer();

	private:
		std::shared_ptr<GraphicsDevice> m_device;

		// ���� ���ҽ� ����
	};
}