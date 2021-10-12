#include "Renderer.h"
#include "../DiveCore.h"
#include "../Log.h"
#include "../TextMesh.h"
#include "Graphics/dvRenderTexture.h"
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

			// OP붙은 것들이 GBuffer와 다르다.
			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// 얻어서 할 것인가, 직접 넘길 것인가...
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
			CORE_ERROR("BufferFrame 생성에 실패하였습니다.");
			return false;
		}

		m_pBufferFrameGPU = new ConstantBuffer(m_pGraphicsDevice, "BufferFrameGPU");
		if (!m_pBufferFrameGPU->Create< BufferFrame >())
		{
			CORE_ERROR("BufferFrameGPU 생성에 실패하였습니다.");
			return false;
		}

		m_pBufferObjectGPU = new ConstantBuffer(m_pGraphicsDevice, "BufferObjectGPU");
		if (!m_pBufferObjectGPU->Create< BufferObject >())
		{
			CORE_ERROR("BufferObjectGPU 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	// 스파르탄에선 CreateRenderTextures다.
	// Wicked에선 RenderPath::ResizeBuffers()이다.
	bool Renderer::createRenderTargets()
	{
		// BackBuffer와 어떻게 구분하느냐...
		auto width = m_RenderTargetSize.x;
		auto height = m_RenderTargetSize.y;

		// 다시 크기 확인을 한다.
		// 크기가 맞지 않다고 생성을 안해버리는게 맞나?
		if ((width / 4) == 0 || (height / 4) == 0)
		{
			CORE_WARN("{0:d}x{1:d}는 지원하지 않는 해상도입니다.", width, height);
			return false;
		}

		// renderTargets들을 만든다. 타입때문에 지웠다가 다시 만들어야 한다. 나중에 수정이 필요할듯?
		// 에디터 실행시 hlsl의 cb를 수정할 때 마다 이 곳에서 뻗는다....
		// 렌더 타겟 설정을 안해서일 수 있다.
		if (m_RenderTargets[eRenderTargets::Frame_Ldr])
		{
			delete m_RenderTargets[eRenderTargets::Frame_Ldr];
			m_RenderTargets[eRenderTargets::Frame_Ldr] = nullptr;
		}
		m_RenderTargets[eRenderTargets::Frame_Ldr] = new Texture(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

		// 이건 결국 Resize() 같은건데...
		// 현재 Relese를 구현했으니 수정이 가능할 것 같다.
		if (m_pRenderTarget)
		{
			delete m_pRenderTarget;
			m_pRenderTarget = nullptr;
		}
		m_pRenderTarget = new dvRenderTexture(width, height, 0, DXGI_FORMAT_R16G16B16A16_FLOAT);
		m_pRenderTarget->Create();

		return true;
	}

	// Shader의 생성 자체는 한 곳에 모으는 것이 맞다. Thread를 이용하여 로드하면 더 좋다.
	// 다만 Pass별로 자신이 사용할 Shader를 관리토록 하면 어떨가 싶다.
	// 물론 이는 책의 예제를 참고한 것일 뿐... 참고 엔진에서는 Pass에서 직접 가져와 bind한다.
	bool Renderer::createShaders()
	{
		// Legacy Shader
		{
			// test용 그냥 그리기
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

			// Deferred Shading
			{
				D3D11_INPUT_ELEMENT_DESC desc[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};

				if (!createVertexShader(L"../Assets/Shaders/Legacy/DeferredShading.hlsl", VSTYPE_DEFERRED_SHADING, ILTYPE_POS_TEX_NOR_TAN, desc, arraysize(desc)))
				{
					return false;
				}
				if (!createPixelShader(L"../Assets/Shaders/Legacy/DeferredShading.hlsl", PSTYPE_DEFERRED_SHADING))
				{
					return false;
				}
			}

			// Light
			{

			}

			// GBufferVis??
			{

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
			DV_ASSERT(result);
		}

		// TextMesh를 Component로 만든 후 제거할 코드
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

	// Wicked는 미리 생성 후 DrawXXX() 내부에서 직접 Bind한다.
	// Spartan은 PassXXX() 내부에서 생성 후 BeginRenderPass()를 통해 Bind한다.
	// 이는 하나의 Pass안에서도 PixelShader가 다를 수 있기 때문이다.
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

	// D3D11_INPUT_ELEMENT_DESC이 nullptr일 필요가 없기에 참조로 받는편이 낫다.
	bool Renderer::createVertexShader(const std::wstring& filepath, unsigned int shaderType, unsigned int inputLayoutType,
		D3D11_INPUT_ELEMENT_DESC* descs, UINT numElements)
	{
		auto pDevice = m_pGraphicsDevice->GetDevice();
		DV_ASSERT(pDevice != nullptr);

		ID3D10Blob* shaderBuffer = nullptr;
		ID3D10Blob* shaderError = nullptr;

		DWORD compileFlags = 0;
#if _DEBUG
		compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_PREFER_FLOW_CONTROL;
#else
		compileFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCIMPILE_OPTIMIZATION_LEVEL3;
#endif

		if (FAILED(D3DCompileFromFile(filepath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "mainVS", "vs_5_0", compileFlags, 0, &shaderBuffer, &shaderError)))
		{
			if (shaderError != nullptr)
			{
				CORE_ERROR("{:s}", (char*)shaderError->GetBufferPointer());
				shaderError->Release(); shaderError = nullptr;
			}
			else
			{
				// 나중에 찾아보자.
				//CORE_ERROR("파일({:s})을 열 수 없습니다.", filepath);
			}

			return false;
		}
		
		if (FAILED(pDevice->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr,
			(ID3D11VertexShader**)m_pShaders[shaderType].GetAddressOf())))
		{
			CORE_ERROR("Vertex Shader 생성에 실패하였습니다.");
			return false;
		}
		
		// 이미 생성 되어 있는 경우 nullptr이 가능해야 한다.
		// 물론 이런 경우가 발생하지 않을 수도 있다.
		if ((descs != nullptr) && (inputLayoutType != ILTYPE_COUNT))
		{
			if (FAILED(pDevice->CreateInputLayout(descs, numElements, shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(),
				m_pInputLayouts[inputLayoutType].GetAddressOf())))
			{
				CORE_ERROR("Input Layout 생성에 실패하였습니다.");
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

		DWORD compileFlags = 0;
#if _DEBUG
		compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_PREFER_FLOW_CONTROL;
#else
		compileFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCIMPILE_OPTIMIZATION_LEVEL3;
#endif

		if (FAILED(D3DCompileFromFile(filepath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "mainPS", "ps_5_0", compileFlags, 0, &shaderBuffer, &shaderError)))
		{
			if (shaderError != nullptr)
			{
				CORE_ERROR("{:s}", (char*)shaderError->GetBufferPointer());
				shaderError->Release(); shaderError = nullptr;
			}
			else
			{
				// 이것두 역시 안된다.
				//CORE_ERROR("파일({})을 열 수 없습니다.", filepath.c_str());
			}

			return false;
		}

		if (FAILED(pDevice->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr,
			(ID3D11PixelShader**)m_pShaders[shaderType].GetAddressOf())))
		{
			CORE_ERROR("Pixel Shader 생성에 실패하였습니다.");
		}

		shaderBuffer->Release(); shaderBuffer = nullptr;

		return true;
	}
}