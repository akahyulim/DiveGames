#pragma once
#include "Graphics/dvGraphicsDevice.h"

namespace dive
{
	class dvRenderer
	{
	public:
		dvRenderer() = default;
		~dvRenderer();

		bool Initialize(const GraphicsData& data);

		dvGraphicsDevice* GetGraphicsDevice() { return m_pGraphicsDevice; }

	private:
	private:
		dvGraphicsDevice* m_pGraphicsDevice = nullptr;
	};
}