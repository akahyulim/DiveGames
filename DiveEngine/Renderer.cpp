#include "Renderer.h"
#include <assert.h>

namespace Dive
{
	Renderer::~Renderer()
	{

	}

	void Renderer::Initialize()
	{
		// state ���� �Լ� ȣ��
		// buffer ���� �Լ� ȣ��
	}

	void Renderer::SetDevice(std::shared_ptr<GraphicsDevice> device)
	{
		m_device = device;

		assert(m_device);

		// �̸� �̿��� �۾� ����
	}
}