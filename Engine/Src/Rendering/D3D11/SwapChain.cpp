#include "DivePch.h"
#include "SwapChain.h"
#include "Core/Log.h"

namespace Dive
{
	SwapChain::SwapChain()
	{
	}

	SwapChain::~SwapChain()
	{
	}

	void SwapChain::ResizeResolution(int width, int height)
	{
		m_Width = width;
		m_Height = height;

		CORE_TRACE("Resize Resolution Width: {0:d}, Height: {1:d}", m_Width, m_Height);
	}
}
