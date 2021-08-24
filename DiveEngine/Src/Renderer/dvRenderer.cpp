#include "dvRenderer.h"
#include "../DiveCore.h"
#include "../Log.h"

namespace dive
{
	dvRenderer::~dvRenderer()
	{
		DV_DELETE(m_pGraphicsDevice);
	}

	// 매개변수 구조체가 마음에 들지 않는다.
	bool dvRenderer::Initialize(const GraphicsData& data)
	{
		m_pGraphicsDevice = new dvGraphicsDevice(data);
		if (!m_pGraphicsDevice->IsInitialized())
		{
			CORE_ERROR("GraphicsDevice 생성 및 초기화에 실패하였습니다.");
			return false;
		}

		// 각종 리소스 생성

		return true;
	}
}