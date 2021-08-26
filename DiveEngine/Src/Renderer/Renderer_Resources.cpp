#include "Renderer.h"
#include "../DiveCore.h"
#include "../Log.h"
#include "../TextMesh.h"
#include <d3dcompiler.h>

using namespace std;

namespace dive
{
	void Renderer::createDepthStencilStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice != nullptr);

		HRESULT hr = E_FAIL;

		{
			D3D11_DEPTH_STENCIL_DESC desc;
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;

			desc.StencilEnable = TRUE;
			desc.StencilReadMask = 0xFF;
			desc.StencilWriteMask = 0xFF;

			// OP���� �͵��� GBuffer�� �ٸ���.
			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// �� �� ���ΰ�, ���� �ѱ� ���ΰ�...
			hr = pDevice->CreateDepthStencilState(&desc, m_pDepthStencilStates[static_cast<int>(eDepthStencilState::Default)].GetAddressOf());
			DV_ASSERT(SUCCEEDED(hr));
		}
	}

	void Renderer::createRasterizerStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice != nullptr);

		HRESULT hr = E_FAIL;

		// CullBack Solid
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

			hr = pDevice->CreateRasterizerState(&desc, m_pRasterizerStates[static_cast<int>(eRasterizerState::Cullback_Solid)].GetAddressOf());
			DV_ASSERT(SUCCEEDED(hr));
		}
	}

	void Renderer::createSamplerStates()
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice != nullptr);

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

			hr = pDevice->CreateSamplerState(&desc, m_pSamplerStates[static_cast<int>(eSamplerState::Default)].GetAddressOf());
			DV_ASSERT(SUCCEEDED(hr));
		}

		{
			D3D11_SAMPLER_DESC desc;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			desc.BorderColor[0] = 0.0f;
			desc.BorderColor[1] = 0.0f;
			desc.BorderColor[2] = 0.0f;
			desc.BorderColor[3] = 0.0f;
			desc.MinLOD = 0;
			desc.MaxLOD = D3D11_FLOAT32_MAX;

			hr = pDevice->CreateSamplerState(&desc, m_pSamplerStates[static_cast<int>(eSamplerState::Linear)].GetAddressOf());
			DV_ASSERT(SUCCEEDED(hr));
		}
	}

	bool Renderer::createConstantBuffers()
	{
		/*
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice != nullptr);

		// world, view, projection matrix
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(MatrixBuffer);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		auto hr = pDevice->CreateBuffer(&desc, nullptr, mConstantBufferMatrix.GetAddressOf());
		DV_ASSERT(SUCCEEDED(hr));
	*/
		m_pBufferFrame = new ConstantBuffer(m_pGraphicsDevice, "BufferFrame");
		if (!m_pBufferFrame->Create< MatrixBuffer>())
		{
			CORE_ERROR("BufferFrame ������ �����Ͽ����ϴ�.");
			return false;
		}

		m_pBufferFrameGPU = new ConstantBuffer(m_pGraphicsDevice, "BufferFrameGPU");
		if (!m_pBufferFrameGPU->Create< BufferFrame >())
		{
			CORE_ERROR("BufferFrameGPU ������ �����Ͽ����ϴ�.");
			return false;
		}

		m_pBufferObjectGPU = new ConstantBuffer(m_pGraphicsDevice, "BufferObjectGPU");
		if (!m_pBufferObjectGPU->Create< BufferObject >())
		{
			CORE_ERROR("BufferObjectGPU ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}

	// ���ĸ�ź���� CreateRenderTextures��.
	bool Renderer::createRenderTargets()
	{
		// BackBuffer�� ��� �����ϴ���...
		auto width = m_RenderTargetSize.x;
		auto height = m_RenderTargetSize.y;

		// �ٽ� ũ�� Ȯ���� �Ѵ�.
		// ũ�Ⱑ ���� �ʴٰ� ������ ���ع����°� �³�?
		if ((width / 4) == 0 || (height / 4) == 0)
		{
			CORE_WARN("{0:d}x{1:d}�� �������� �ʴ� �ػ��Դϴ�.", width, height);
			return false;
		}

		// renderTargets���� �����. Ÿ�Զ����� �����ٰ� �ٽ� ������ �Ѵ�. ���߿� ������ �ʿ��ҵ�?
		// ������ ����� hlsl�� cb�� ������ �� ���� �� ������ ���´�....
		// ���� Ÿ�� ������ ���ؼ��� �� �ִ�.
		if (m_RenderTargets[eRenderTargets::Frame_Ldr])
		{
			delete m_RenderTargets[eRenderTargets::Frame_Ldr];
			m_RenderTargets[eRenderTargets::Frame_Ldr] = nullptr;
		}
		m_RenderTargets[eRenderTargets::Frame_Ldr] = new Texture(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

		return true;
	}

	bool Renderer::createShaders()
	{
		// Legacy Shader
		{
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			if (!createVertexShader(L"../DiveEngine/legacy.hlsl", VSTYPE_LEGACY, ILTYPE_POS_TEX_NOR_TAN, desc, arraysize(desc)))
			{
				return false;
			}
			if (!createPixelShader(L"../DiveEngine/legacy.hlsl", PSTYPE_LEGACY))
			{
				return false;
			}
		}

		// Font Shader
		{
			// ���� InputLayout��� �޸� �ϳ��� VertexType ����ü�� ������� �ִ�.
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			if (!createVertexShader(L"../DiveEngine/fonts.hlsl", VSTYPE_FONTS, ILTYPE_POS_TEX2, desc, arraysize(desc)))
			{
				return false;
			}
			if (!createPixelShader(L"../DiveEngine/fonts.hlsl", PSTYPE_FONTS))
			{
				return false;
			}
		}

		return true;
	}

	bool Renderer::createFonts()
	{
		auto result = false;

		{
			m_pDvFont = new Font;
			result = m_pDvFont->LoadFromFile("../Assets/Fonts/NanumBarunGothic.ttf");
			DV_ASSERT(result);
		}

		// TextMesh�� Component�� ���� �� ������ �ڵ�
		/*
		{
			m_pTextMesh = new TextMesh;
			result = m_pTextMesh->SetFont(m_pDvFont);
			DV_ASSERT(result);
			m_pTextMesh->SetText(L"We're just walking down the street.",
				DirectX::XMFLOAT2(-200.0f, 200.0f));
		}
		*/

		return result;
	}

	void Renderer::createPipelineStates()
	{
		// Legacy
		{
			m_PipelineStateLegacy.pVS = (ID3D11VertexShader*)m_pShaders[VSTYPE_LEGACY].Get();
			m_PipelineStateLegacy.pPS = (ID3D11PixelShader*)m_pShaders[PSTYPE_LEGACY].Get();
			m_PipelineStateLegacy.pDSS = GetDepthStencilState(eDepthStencilState::Default);
			m_PipelineStateLegacy.pRSS = GetRasterizerState(eRasterizerState::Cullback_Solid);
			m_PipelineStateLegacy.pSS = GetSamplerState(eSamplerState::Default);
			m_PipelineStateLegacy.pIL = m_pInputLayouts[ILTYPE_POS_TEX_NOR_TAN].Get();
			m_PipelineStateLegacy.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}

		// Font
		{
			m_PipelineStateFont.pVS = (ID3D11VertexShader*)m_pShaders[VSTYPE_FONTS].Get();
			m_PipelineStateFont.pPS = (ID3D11PixelShader*)m_pShaders[PSTYPE_FONTS].Get();
			m_PipelineStateFont.pDSS = GetDepthStencilState(eDepthStencilState::Default);
			m_PipelineStateFont.pRSS = GetRasterizerState(eRasterizerState::Cullback_Solid);
			m_PipelineStateFont.pSS = GetSamplerState(eSamplerState::Linear);
			m_PipelineStateFont.pIL = m_pInputLayouts[ILTYPE_POS_TEX2].Get();
			m_PipelineStateFont.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
	}

	// D3D11_INPUT_ELEMENT_DESC�� nullptr�� �ʿ䰡 ���⿡ ������ �޴����� ����.
	bool Renderer::createVertexShader(const std::wstring& filepath, unsigned int shaderType, unsigned int inputLayoutType,
		D3D11_INPUT_ELEMENT_DESC* descs, UINT numElements)
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice != nullptr);

		ID3D10Blob* shaderBuffer = nullptr;
		ID3D10Blob* shaderError = nullptr;

		if (FAILED(D3DCompileFromFile(filepath.c_str(), NULL, NULL, "mainVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &shaderError)))
		{
			if (shaderError != nullptr)
			{
				CORE_ERROR("{:s}", (char*)shaderError->GetBufferPointer());
				shaderError->Release(); shaderError = nullptr;
			}
			else
			{
				// ���߿� ã�ƺ���.
				//CORE_ERROR("����({:s})�� �� �� �����ϴ�.", filepath);
			}

			return false;
		}
		
		if (FAILED(pDevice->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr,
			(ID3D11VertexShader**)m_pShaders[shaderType].GetAddressOf())))
		{
			CORE_ERROR("Vertex Shader ������ �����Ͽ����ϴ�.");
			return false;
		}
		
		if ((descs != nullptr) && (inputLayoutType != ILTYPE_COUNT))
		{
			if (FAILED(pDevice->CreateInputLayout(descs, numElements, shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(),
				m_pInputLayouts[inputLayoutType].GetAddressOf())))
			{
				CORE_ERROR("Input Layout ������ �����Ͽ����ϴ�.");
				return false;
			}
		}
		shaderBuffer->Release(); shaderBuffer = nullptr;
		
		return true;
	}

	bool Renderer::createPixelShader(const std::wstring& filepath, unsigned int shaderType)
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice != nullptr);

		ID3D10Blob* shaderBuffer = nullptr;
		ID3D10Blob* shaderError = nullptr;

		if (FAILED(D3DCompileFromFile(filepath.c_str(), NULL, NULL, "mainPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &shaderBuffer, &shaderError)))
		{
			if (shaderError != nullptr)
			{
				CORE_ERROR("{:s}", (char*)shaderError->GetBufferPointer());
				shaderError->Release(); shaderError = nullptr;
			}
			else
			{
				// �̰͵� ���� �ȵȴ�.
				//CORE_ERROR("����({})�� �� �� �����ϴ�.", filepath.c_str());
			}

			return false;
		}

		if (FAILED(pDevice->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr,
			(ID3D11PixelShader**)m_pShaders[shaderType].GetAddressOf())))
		{
			CORE_ERROR("Pixel Shader ������ �����Ͽ����ϴ�.");
		}

		shaderBuffer->Release(); shaderBuffer = nullptr;

		return true;
	}
}