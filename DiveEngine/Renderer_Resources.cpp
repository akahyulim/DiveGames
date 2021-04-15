#include "Renderer.h"
#include "Log.h"
#include <d3dcompiler.h>

using namespace std;

namespace Dive
{
	bool Renderer::createDepthStencilStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		assert(pDevice != nullptr);

		HRESULT hr = E_FAIL;

		// DepthStenicl States
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

		return true;
	}

	bool Renderer::createRasterizerStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		assert(pDevice != nullptr);

		HRESULT hr = E_FAIL;

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

	bool Renderer::createSamplerStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		assert(pDevice != nullptr);

		HRESULT hr = E_FAIL;

		// ��� ������ ���ΰ��� ���ؾ� �Ѵ�.
		{
			D3D11_SAMPLER_DESC desc;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			desc.BorderColor[0] = 0.0f;
			desc.BorderColor[1] = 0.0f;
			desc.BorderColor[2] = 0.0f;
			desc.BorderColor[3] = 0.0f;
			desc.MinLOD = 0;
			desc.MaxLOD = D3D11_FLOAT32_MAX;

			hr = pDevice->CreateSamplerState(&desc, m_pSamplerState.GetAddressOf());
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

	// ������ �����Ϳ��� ����ϴ� ����Ʈ �ؽ��ĸ� �����Ѵ�.
	bool Renderer::createTextures()
	{
		m_pTex = std::make_shared<Texture>(L"../Assets/Textures/Choa.jpg");

		return true;
	}

	// �̸��� �� �ָ��ϴ�. dsv�� �����ҰŴ�.
	bool Renderer::createRenderTargetViews()
	{
		auto width = m_pGraphicsDevice->GetResolutionWidth();
		auto height = m_pGraphicsDevice->GetResolutionHeight();

		m_pRTV = std::make_shared<Texture>(width, height, DXGI_FORMAT_R8G8B8A8_UINT, "Test_RenderTargetView");

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

		// Texturing Shader
		{
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			if (!createVertexShader(L"../DiveEngine/texturing.hlsl", VSTYPE_TEXTURING, ILTYPE_POS_TEX, desc, arraysize(desc)))
			{
				return false;
			}
			if (!createPixelShader(L"../DiveEngine/texturing.hlsl", PSTYPE_TEXTURING))
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

		// Texturing
		{
			m_pipelineStateTexturing.pVS = (ID3D11VertexShader*)m_pShaders[VSTYPE_TEXTURING].Get();
			m_pipelineStateTexturing.pPS = (ID3D11PixelShader*)m_pShaders[PSTYPE_TEXTURING].Get();
			m_pipelineStateTexturing.pDSS = m_pDepthStencilStates[DSSTYPE_DEFAULT].Get();
			m_pipelineStateTexturing.pRSS = m_pRasterizerStates[RSSTYPE_CULLBACK_SOLID].Get();
			m_pipelineStateTexturing.pSS = m_pSamplerState.Get();
			m_pipelineStateTexturing.pIL = m_pInputLayouts[ILTYPE_POS_TEX].Get();
			m_pipelineStateTexturing.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
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
				// ���߿� ã�ƺ���.
				//CORE_ERROR("����({:s})�� �� �� �����ϴ�.", filepath);
			}

			return false;
		}
		
		if (FAILED(pDevice->CreateVertexShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr,
			(ID3D11VertexShader**)m_pShaders[shaderType].GetAddressOf())))
		{
			CORE_ERROR("Vertex Shader ������ �����Ͽ����ϴ�.");
			return false;
		}
		
		if ((pDescs != nullptr) && (inputLayoutType != ILTYPE_COUNT))
		{
			if (FAILED(pDevice->CreateInputLayout(pDescs, numElements, pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(),
				m_pInputLayouts[inputLayoutType].GetAddressOf())))
			{
				CORE_ERROR("Input Layout ������ �����Ͽ����ϴ�.");
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
				// �̰͵� ���� �ȵȴ�.
				//CORE_ERROR("����({})�� �� �� �����ϴ�.", filepath.c_str());
			}

			return false;
		}

		if (FAILED(pDevice->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr,
			(ID3D11PixelShader**)m_pShaders[shaderType].GetAddressOf())))
		{
			CORE_ERROR("Pixel Shader ������ �����Ͽ����ϴ�.");
		}

		pShaderBuffer->Release(); pShaderBuffer = nullptr;

		return true;
	}
}