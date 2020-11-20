#pragma once
#include "core/Object.h"


namespace Dive
{
	class RenderDevice;

	class BlendState
	{
	public:
		BlendState(RenderDevice* device,
			BOOL enable					= FALSE,
			D3D11_BLEND source			= D3D11_BLEND_SRC_ALPHA,
			D3D11_BLEND dest			= D3D11_BLEND_INV_SRC_ALPHA,
			D3D11_BLEND_OP op			= D3D11_BLEND_OP_ADD,
			D3D11_BLEND sourceAlpha		= D3D11_BLEND_ONE,
			D3D11_BLEND destAlpha		= D3D11_BLEND_ONE,
			D3D11_BLEND_OP opAlpha		= D3D11_BLEND_OP_ADD,
			float* blendFactor			= nullptr);
		~BlendState();

		bool IsInitialized()			const { return m_bInitialized; }
		ID3D11BlendState* GetState()	const { return m_State; }
		const float* GetBlendFactor()	const { return &m_BlendFactor[0]; }

	private:
		bool m_bInitialized			= false;
		ID3D11BlendState* m_State	= nullptr;
		float m_BlendFactor[4]		= { 1, };
	};
}

