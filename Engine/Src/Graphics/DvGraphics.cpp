#include "divepch.h"
#include "DvGraphics.h"
#include "Core/DvContext.h"

Dive::DvGraphics::DvGraphics(DvContext* pContext)
	: DvObject(pContext)
{
}

Dive::DvGraphics::~DvGraphics()
{
}

bool Dive::DvGraphics::IsInitialized()
{
	// window, device, context À¯¹«

	return false;
}

bool Dive::DvGraphics::BeginFrame()
{
	if(!IsInitialized())
		return false;

	// reset render targets?

	return true;
}

void Dive::DvGraphics::EndFrame()
{
	if (!IsInitialized())
		return;

	// swapchain present
}
