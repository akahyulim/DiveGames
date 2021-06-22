#include "Renderer.h"
#include "Log.h"
#include "TextMesh.h"
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

		// 어떻게 구분할 것인가를 정해야 한다.
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

			hr = pDevice->CreateSamplerState(&desc, m_pSamplerStateLinear.GetAddressOf());
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

	// 기존 사용처와 성격이 달라졌다.
	// 스파르탄에선 에디터용 SRV 생성에 사용되었다.
	bool Renderer::createTextures()
	{
		// 삭제 대상 : 테스트용 구문
		{
			m_pTex = std::make_shared<Texture>(L"../Assets/Textures/Choa.jpg");

			// 생성한 후 특정 색 넣기
			{
				m_pCpuTex = std::make_shared<Texture>(800, 600);

				auto pImmediateContex = m_pGraphicsDevice->GetImmediateContext();

				D3D11_MAPPED_SUBRESOURCE mappedResource;
				pImmediateContex->Map(m_pCpuTex->GetTexture2D(),
					D3D11CalcSubresource(0, 0, 1),
					D3D11_MAP_WRITE_DISCARD,
					0,
					&mappedResource);

				UCHAR* pTexels = (UCHAR*)mappedResource.pData;

				// 색상 넣기
				for (UINT row = 0; row < 600; ++row)
				{
					UINT rowStart = row * mappedResource.RowPitch;

					for (UINT col = 0; col < 800; ++col)
					{
						UINT colStart = col * 4;

						pTexels[rowStart + colStart + 0] = 100;		// R
						pTexels[rowStart + colStart + 1] = 0; 128;		// G
						pTexels[rowStart + colStart + 2] = 0; 64;		// B
						pTexels[rowStart + colStart + 3] = 0; 32;
					}
				}

				pImmediateContex->Unmap(m_pCpuTex->GetTexture2D(), D3D11CalcSubresource(0, 0, 1));
			}
		}

		return true;
	}

	// 스파르탄에선 CreateRenderTextures다.
	bool Renderer::createRenderTargets()
	{
		// BackBuffer와 어떻게 구분하느냐...
		auto width = m_resolution.x;
		auto height = m_resolution.y;

		// 다시 크기 확인을 한다.
		// 크기가 맞지 않다고 생성을 안해버리는게 맞나?
		if ((width / 4) == 0 || (height / 4) == 0)
		{
			CORE_WARN("{0:d}x{1:d}는 지원하지 않는 해상도입니다.", width, height);
			return false;
		}

		// 얘는 테스트용이다.
		{
//			m_pRTV = std::make_shared<Texture>(width, height, DXGI_FORMAT_R8G8B8A8_UINT, "Test_RenderTargetView");
		}

		// renderTargets들을 만든다. 타입때문에 지웠다가 다시 만들어야 한다. 나중에 수정이 필요할듯?
		if (m_renderTargets[eRenderTargets::Frame_Ldr])
			delete m_renderTargets[eRenderTargets::Frame_Ldr];
		m_renderTargets[eRenderTargets::Frame_Ldr] = new Texture(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

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
			// 각 요소마다 Buffer가 다르기때문에 inputSlot은 스택을 쌓아야 한다.
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

		// Font Shader
		{
			// 위의 InputLayout들과 달리 하나의 VertexType 구조체로 만들어져 있다.
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
			assert(result);
		}

		// TextMesh를 Component로 만든 후 제거할 코드
		/*
		{
			m_pTextMesh = new TextMesh;
			result = m_pTextMesh->SetFont(m_pDvFont);
			assert(result);
			m_pTextMesh->SetText(L"We're just walking down the street.",
				DirectX::XMFLOAT2(-200.0f, 200.0f));
		}
		*/

		return result;
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

		// Font
		{
			m_pipelineStateFont.pVS = (ID3D11VertexShader*)m_pShaders[VSTYPE_FONTS].Get();
			m_pipelineStateFont.pPS = (ID3D11PixelShader*)m_pShaders[PSTYPE_FONTS].Get();
			m_pipelineStateFont.pDSS = m_pDepthStencilStates[DSSTYPE_DEFAULT].Get();
			m_pipelineStateFont.pRSS = m_pRasterizerStates[RSSTYPE_CULLBACK_SOLID].Get();
			m_pipelineStateFont.pSS = m_pSamplerStateLinear.Get();
			m_pipelineStateFont.pIL = m_pInputLayouts[ILTYPE_POS_TEX2].Get();
			m_pipelineStateFont.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
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