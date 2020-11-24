#pragma once
#include "Core/Object.h"
#include "RenderDeviceEnums.h"

namespace Dive
{
	class RenderDevice;

	class DepthStencilState : public Object
	{
	public:
		DepthStencilState(const std::shared_ptr<RenderDevice>& device, BOOL enable, D3D11_COMPARISON_FUNC cmpFunc);
		~DepthStencilState();

		bool IsInitialized()				const { return m_bInitialized; }
		ID3D11DepthStencilState* GetState() const { return m_State; }

	private:
		bool m_bInitialized					= false;
		ID3D11DepthStencilState* m_State	= nullptr;
	};
}
