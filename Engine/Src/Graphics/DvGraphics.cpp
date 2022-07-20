#include "divepch.h"
#include "DvGraphics.h"
#include "Core/DvContext.h"

Dive::DvGraphics::DvGraphics(DvContext* pContext)
	: DvObject(pContext),
	m_pSwapChain(nullptr),
	m_pDevice(nullptr),
	m_pImmediateContext(nullptr)
{
	std::cout << "Graphics 생성자" << std::endl;

	// d3d11 생성
	// window handle, size 등이 필요
}

Dive::DvGraphics::~DvGraphics()
{
	std::cout << "Graphics 소멸자" << std::endl;
}

bool Dive::DvGraphics::IsInitialized()
{
	// window, device, context 유무
	if (!m_pDevice || !m_pImmediateContext)
		return false;

	return true;
}

bool Dive::DvGraphics::BeginFrame()
{
	std::cout << "Graphics BeginFrame" << std::endl;

	if(!IsInitialized())
		return false;

	// reset render targets?

	return true;
}

void Dive::DvGraphics::EndFrame()
{
	std::cout << "Graphics EndFrame" << std::endl;

	if (!IsInitialized())
		return;

	// swapchain present
}
