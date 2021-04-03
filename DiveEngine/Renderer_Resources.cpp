#include "Renderer.h"
#include "Log.h"
#include <d3dcompiler.h>

using namespace std;

namespace Dive
{
	bool Renderer::createStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		assert(pDevice != nullptr);

		HRESULT hr;

		// DepthStenicl State
		{
			D3D11_DEPTH_STENCIL_DESC desc;
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;

			desc.StencilEnable = true;
			desc.StencilReadMask = 0xFF;
			desc.StencilWriteMask = 0xFF;

			// Stencil operations if pixel is front-facing.
			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil operations if pixel is back-facing.
			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			hr = pDevice->CreateDepthStencilState(&desc, m_pDepthStencilStates[DSSTYPE_DEFAULT].GetAddressOf());
			assert(SUCCEEDED(hr));
		}

		// Rasterizer State
		{
			D3D11_RASTERIZER_DESC desc;
			desc.AntialiasedLineEnable = false;
			desc.CullMode = D3D11_CULL_BACK;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0.0f;
			desc.DepthClipEnable = true;
			desc.FillMode = D3D11_FILL_SOLID;
			desc.FrontCounterClockwise = false;
			desc.MultisampleEnable = false;
			desc.ScissorEnable = false;
			desc.SlopeScaledDepthBias = 0.0f;

			hr = pDevice->CreateRasterizerState(&desc, m_pRasterizerStates[RSSTYPE_CULLBACK_SOLID].GetAddressOf());
			assert(SUCCEEDED(hr));
		}

		return true;
	}

	bool Renderer::createConstantBuffers()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		assert(pDevice != nullptr);

		// world, view, projection matrix
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(MatrixBuffer);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		auto hr = pDevice->CreateBuffer(&desc, nullptr, m_pCBMatrix.GetAddressOf());
		assert(SUCCEEDED(hr));
	
		return true;
	}

	bool Renderer::createTextures()
	{
		return true;
	}

	bool Renderer::createShaders()
	{
		// Color Shader
		{
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			if (!createVertexShader(L"../DiveEngine/color.hlsl", VSTYPE_COLOR, ILTYPE_POS_COL, desc, arraysize(desc)))
			{
				return false;
			}
			if (!createPixelShader(L"../DiveEngine/color.hlsl", PSTYPE_COLOR))
			{
				return false;
			}
		}

		return true;
	}

	void Renderer::createPipelineStates()
	{
		// Color
		{
			m_pipelineStateColor.pVS = (ID3D11VertexShader*)m_pShaders[VSTYPE_COLOR].Get();
			m_pipelineStateColor.pPS = (ID3D11PixelShader*)m_pShaders[PSTYPE_COLOR].Get();
			m_pipelineStateColor.pDSS = m_pDepthStencilStates[DSSTYPE_DEFAULT].Get();
			m_pipelineStateColor.pRSS = m_pRasterizerStates[RSSTYPE_CULLBACK_SOLID].Get();
			m_pipelineStateColor.pIL = m_pInputLayouts[ILTYPE_POS_COL].Get();
			m_pipelineStateColor.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
	}

	bool Renderer::createVertexShader(const std::wstring& filepath, unsigned int shaderType, unsigned int inputLayoutType,
		D3D11_INPUT_ELEMENT_DESC* pDescs, UINT numElements)
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		assert(pDevice != nullptr);

		ID3D10Blob* pShaderBuffer = nullptr;
		ID3D10Blob* pShaderError = nullptr;

		if (FAILED(D3DCompileFromFile(filepath.c_str(), NULL, NULL, "mainVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pShaderBuffer, &pShaderError)))
		{
			if (pShaderError != nullptr)
			{
				CORE_ERROR("{:s}", (char*)pShaderError->GetBufferPointer());
				pShaderError->Release(); pShaderError = nullptr;
			}
			else
			{
				// 나중에 찾아보자.
				//CORE_ERROR("파일({:s})을 열 수 없습니다.", filepath);
			}

			return false;
		}
		
		if (FAILED(pDevice->CreateVertexShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr,
			(ID3D11VertexShader**)m_pShaders[shaderType].GetAddressOf())))
		{
			CORE_ERROR("Vertex Shader 생성에 실패하였습니다.");
			return false;
		}
		
		if ((pDescs != nullptr) && (inputLayoutType != ILTYPE_COUNT))
		{
			if (FAILED(pDevice->CreateInputLayout(pDescs, numElements, pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(),
				m_pInputLayouts[inputLayoutType].GetAddressOf())))
			{
				CORE_ERROR("Input Layout 생성에 실패하였습니다.");
				return false;
			}
		}
		pShaderBuffer->Release(); pShaderBuffer = nullptr;
		
		return true;
	}

	bool Renderer::createPixelShader(const std::wstring& filepath, unsigned int shaderType)
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		assert(pDevice != nullptr);

		ID3D10Blob* pShaderBuffer = nullptr;
		ID3D10Blob* pShaderError = nullptr;

		if (FAILED(D3DCompileFromFile(filepath.c_str(), NULL, NULL, "mainPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pShaderBuffer, &pShaderError)))
		{
			if (pShaderError != nullptr)
			{
				CORE_ERROR("{:s}", (char*)pShaderError->GetBufferPointer());
				pShaderError->Release(); pShaderError = nullptr;
			}
			else
			{
				// 이것두 역시 안된다.
				//CORE_ERROR("파일({})을 열 수 없습니다.", filepath.c_str());
			}

			return false;
		}

		if (FAILED(pDevice->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr,
			(ID3D11PixelShader**)m_pShaders[shaderType].GetAddressOf())))
		{
			CORE_ERROR("Pixel Shader 생성에 실패하였습니다.");
		}

		pShaderBuffer->Release(); pShaderBuffer = nullptr;

		return true;
	}
}