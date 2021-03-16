#include "Renderer.h"
#include "Log.h"
#include <assert.h>

namespace Dive
{
	Renderer::~Renderer()
	{
		// 리소스 제거
	}

	void Renderer::Initialize()
	{
		if (!m_device || !m_device->IsInitialized())
		{
			CORE_ERROR("Graphics Device가 생성되지 않아 초기화를 실행할 수 없습니다. 프로그램을 종료합니다.");
			PostQuitMessage(0);
		}

		// state 생성 함수 호출
		// buffer 생성 함수 호출 = resource
		// shader 생성 함수 호출(컴파일된 cso 파일을 이용한다.)

		
	}

	void Renderer::SetDevice(std::shared_ptr<GraphicsDevice> device)
	{
		m_device = device;

		assert(m_device);

		// 이를 이용한 작업 수행
	}
}