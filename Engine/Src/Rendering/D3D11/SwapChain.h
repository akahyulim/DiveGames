#pragma once

namespace Dive
{
	class SwapChain
	{
	public:
		SwapChain();
		~SwapChain();

		void ResizeResolution(int width, int height);

	private:
		int m_Width;
		int m_Height;
	};
}

