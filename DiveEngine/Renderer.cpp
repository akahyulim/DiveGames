#include "Renderer.h"
#include <assert.h>

namespace Dive
{
	Renderer::~Renderer()
	{

	}

	void Renderer::Initialize()
	{
		// state 생성 함수 호출
		// buffer 생성 함수 호출
	}

	void Renderer::SetDevice(std::shared_ptr<GraphicsDevice> device)
	{
		m_device = device;

		assert(m_device);

		// 이를 이용한 작업 수행
	}
}