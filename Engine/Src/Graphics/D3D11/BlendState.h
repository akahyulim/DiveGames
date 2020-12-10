#pragma once
#include "core/Object.h"


namespace Dive
{
	class Context;

	class BlendState : public Object
	{
		DIVE_OBJECT(BlendState, Object);

	public:
		BlendState(Context* context,
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
		ID3D11BlendState* GetState()	const { return m_state; }
		const float* GetBlendFactor()	const { return &m_blendFactor[0]; }

	private:
		bool m_bInitialized			= false;
		ID3D11BlendState* m_state	= nullptr;
		float m_blendFactor[4]		= { 1, };
	};
}

