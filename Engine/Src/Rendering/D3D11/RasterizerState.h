#pragma once
#include "Core/Object.h"
#include "RenderDeviceEnums.h"


namespace Dive
{
	class RenderDevice;

	class RasterizerState : public Object
	{
	public:
		RasterizerState(const std::shared_ptr<RenderDevice>& device, D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, BOOL depthClipEnable, BOOL scissorEnable,
			BOOL multiSampleEnable, BOOL antialiasedLineEnable);
		~RasterizerState();

		bool IsInitialized()				const { return m_bInitialized; }
		ID3D11RasterizerState* GetState()	const { return m_State; }

	private:
		bool m_bInitialized				= false;
		ID3D11RasterizerState* m_State	= nullptr;
	};
}
