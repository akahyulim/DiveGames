#include "DivePch.h"
#include "Graphics.h"
#include "Graphics_ConstantBuffers.h"
#include "Core/Log.h"
#include "D3D11/DepthStencilState.h"
#include "D3D11/BlendState.h"
#include "D3D11/RasterizerState.h"
#include "D3D11/Sampler.h"
#include "D3D11/Texture2D.h"
#include "D3D11/Shader.h"
#include "D3D11/ConstantBuffer.h"

namespace Dive
{
	bool Graphics::createDepthStencilStates()
	{
		m_depthStencilStateEnabled = std::make_shared<DepthStencilState>(m_context, TRUE, D3D11_COMPARISON_LESS);
		m_depthStencilStateDisabled = std::make_shared<DepthStencilState>(m_context, FALSE, D3D11_COMPARISON_LESS);

		if (!m_depthStencilStateEnabled || !m_depthStencilStateDisabled)
		{
			CORE_ERROR("Fail to create depthstencil states.");
			return false;
		}

		return true;
	}

	bool Graphics::createBlendStates()
	{
		m_blendStateEnable = std::make_shared<BlendState>(m_context, TRUE);
		m_blendStateDisable = std::make_shared<BlendState>(m_context, FALSE);
		m_blendStateColorAdd = std::make_shared<BlendState>(m_context, TRUE, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);

		if (!m_blendStateEnable || !m_blendStateDisable || !m_blendStateColorAdd)
		{
			CORE_ERROR("Fail to create beldn states.");
			return false;
		}

		return true;
	}

	bool Graphics::createRasterizerStates()
	{
		m_rasterizeStateCullBackSolid = std::make_shared<RasterizerState>(m_context, D3D11_CULL_BACK, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
		m_rasterizeStateCullFrontSolid = std::make_shared<RasterizerState>(m_context, D3D11_CULL_FRONT, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
		m_rasterizeStateCullNoneSolid = std::make_shared<RasterizerState>(m_context, D3D11_CULL_NONE, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);

		m_rasterizeStateCullBackWireFrame = std::make_shared<RasterizerState>(m_context, D3D11_CULL_BACK, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
		m_rasterizeStateCullFrontWireFrame = std::make_shared<RasterizerState>(m_context, D3D11_CULL_FRONT, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
		m_rasterizeStateCullNoneWireFrame = std::make_shared<RasterizerState>(m_context, D3D11_CULL_NONE, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);

		if (!m_rasterizeStateCullBackSolid || !m_rasterizeStateCullFrontSolid || !m_rasterizeStateCullNoneSolid
			|| !m_rasterizeStateCullBackWireFrame || !m_rasterizeStateCullFrontWireFrame || !m_rasterizeStateCullNoneWireFrame)
		{
			CORE_ERROR("Fail to create rasterizer states.");
			return false;
		}

		return true;
	}

	bool Graphics::createSampels()
	{
		m_samplerLinear = std::make_shared<Sampler>(m_context, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

		m_samplerCompareDepth = std::make_shared<Sampler>(m_context, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_GREATER_EQUAL);
		m_samplerPointClamp = std::make_shared<Sampler>(m_context, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_ALWAYS);
		m_samplerBilinearClamp = std::make_shared<Sampler>(m_context, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
		m_samplerBilinearWrap = std::make_shared<Sampler>(m_context, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
		m_samplerTrilinearClamp = std::make_shared<Sampler>(m_context, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
		m_samplerAnisotropicWrap = std::make_shared<Sampler>(m_context, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 16);

		if (!m_samplerLinear || !m_samplerCompareDepth || !m_samplerPointClamp | !m_samplerBilinearClamp
			|| !m_samplerBilinearWrap || !m_samplerTrilinearClamp || !m_samplerAnisotropicWrap)
		{
			CORE_ERROR("fail to create samplers.");
			return false;
		}

		return true;
	}

	bool Graphics::createShaders()
	{


		return true;
	}

	bool Graphics::createTextures()
	{
		// 각종 RenderTarget과 DepthStencil View를 생성하는 함수 였으나
		// 현재 GBuffer를 따로 만들었다.
		
		return true;
	}

	bool Graphics::createConstantBuffers()
	{
		m_cbObject = std::make_shared<ConstantBuffer>(m_context);
		m_cbObject->Create<CB_OBJECT>();

		return true;
	}
}