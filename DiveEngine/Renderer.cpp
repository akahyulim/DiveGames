#include "Renderer.h"
#include "Log.h"
#include <assert.h>

namespace Dive
{
	Renderer::~Renderer()
	{
		// ���ҽ� ����
	}

	void Renderer::Initialize()
	{
		if (!m_device || !m_device->IsInitialized())
		{
			CORE_ERROR("Graphics Device�� �������� �ʾ� �ʱ�ȭ�� ������ �� �����ϴ�. ���α׷��� �����մϴ�.");
			PostQuitMessage(0);
		}

		// state ���� �Լ� ȣ��
		// buffer ���� �Լ� ȣ�� = resource
		// shader ���� �Լ� ȣ��(�����ϵ� cso ������ �̿��Ѵ�.)

		
	}

	void Renderer::SetDevice(std::shared_ptr<GraphicsDevice> device)
	{
		m_device = device;

		assert(m_device);

		// �̸� �̿��� �۾� ����
	}
}