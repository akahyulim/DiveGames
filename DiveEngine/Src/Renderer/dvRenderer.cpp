#include "dvRenderer.h"
#include "../DiveCore.h"
#include "../Log.h"

namespace dive
{
	dvRenderer::~dvRenderer()
	{
		DV_DELETE(m_pGraphicsDevice);
	}

	// �Ű����� ����ü�� ������ ���� �ʴ´�.
	bool dvRenderer::Initialize(const GraphicsData& data)
	{
		m_pGraphicsDevice = new dvGraphicsDevice(data);
		if (!m_pGraphicsDevice->IsInitialized())
		{
			CORE_ERROR("GraphicsDevice ���� �� �ʱ�ȭ�� �����Ͽ����ϴ�.");
			return false;
		}

		// ���� ���ҽ� ����

		return true;
	}
}