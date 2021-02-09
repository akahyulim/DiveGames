#include "DivePch.h"
#include "Graphics.h"
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
		return false;
	}

	bool Graphics::createTextures()
	{
		
		return true;
	}

	bool Graphics::createBaseShader()
	{
		m_baseShader = new Shader(m_context);
		if (!m_baseShader->CreateShader(eVertexType::PositionUvNormalTangent, L"../Assets/Shaders/baseShader.hlsl"))
			return false;

		return true;
	}

	// 원래는 생성한 후 update로 갱신시켜야 한다.
	bool Graphics::createConstantBuffer()
	{
		m_constantBuffer = new ConstantBuffer(m_context);
		if (!m_constantBuffer->Create<MatrixBuffer>())
			return false;

		// view
		DirectX::XMMATRIX view;
		//		{
		DirectX::XMFLOAT3 pos, lookAt, up;
		pos.x = 0.0f; pos.y = 0.0f; pos.z = -5.0f;
		lookAt.x = 0.0f; lookAt.y = 0.0f; lookAt.z = 0.0f;
		up.x = 0.0f, up.y = 1.0f; up.z = 0.0f;
		DirectX::XMVECTOR vecPos, vecLookAt, vecUp;
		vecPos = DirectX::XMLoadFloat3(&pos);
		vecLookAt = DirectX::XMLoadFloat3(&lookAt);
		vecUp = DirectX::XMLoadFloat3(&up);
		view = DirectX::XMMatrixLookAtLH(vecPos, vecLookAt, vecUp);
		//		}

				// proj
		DirectX::XMMATRIX proj;
		//		{
		float fieldOfView = 3.141592654f / 4.0f;
		float screenAspect = (float)m_textureSize.x / (float)m_textureSize.y;
		proj = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f);
		//		}

		auto ptr = static_cast<MatrixBuffer*>(m_constantBuffer->Map());
		DirectX::XMStoreFloat4x4(&(ptr->world), DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()));
		//DirectX::XMStoreFloat4x4(&(ptr->view), DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()));
		DirectX::XMStoreFloat4x4(&(ptr->view), DirectX::XMMatrixTranspose(view));
		DirectX::XMStoreFloat4x4(&(ptr->proj), DirectX::XMMatrixTranspose(proj));

		if (!m_constantBuffer->Unmap())
			return false;

		return true;
	}
}