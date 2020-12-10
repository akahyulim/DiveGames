#include "DivePch.h"
#include "DepthStencilState.h"
#include "Graphics/Graphics.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"


namespace Dive
{
	DepthStencilState::DepthStencilState(Context* context, BOOL enable, D3D11_COMPARISON_FUNC cmpFunc)
		: Object(context)
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized())
		{
			CORE_ERROR("");
			return;
		}

		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthFunc						= cmpFunc;
		desc.StencilEnable					= false;
		desc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;
		desc.FrontFace.StencilPassOp		= enable ? D3D11_STENCIL_OP_REPLACE : D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
		desc.BackFace						= desc.FrontFace;
		desc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthEnable					= enable;

		if (FAILED(graphics->GetRHIDevice()->CreateDepthStencilState(&desc, &m_state)))
		{
			CORE_ERROR("");
			return;
		}

		m_bInitialized = true;
	}

	DepthStencilState::~DepthStencilState()
	{
		SAFE_RELEASE(m_state);
	}
}