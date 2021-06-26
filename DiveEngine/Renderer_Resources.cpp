#include "Renderer.h"
#include "Log.h"
#include "TextMesh.h"
#include <d3dcompiler.h>

using namespace std;

namespace dive
{
	bool Renderer::createDepthStencilStates()
	{
		auto device = mGraphicsDevice->GetDevice();
		assert(device != nullptr);

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

			hr = device->CreateDepthStencilState(&desc, mDepthStencilStates[DSSTYPE_DEFAULT].GetAddressOf());
			assert(SUCCEEDED(hr));
		}

		return true;
	}

	bool Renderer::createRasterizerStates()
	{
		auto device = mGraphicsDevice->GetDevice();
		assert(device != nullptr);

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

			hr = device->CreateRasterizerState(&desc, mRasterizerStates[RSSTYPE_CULLBACK_SOLID].GetAddressOf());
			assert(SUCCEEDED(hr));
		}

		return true;
	}

	bool Renderer::createSamplerStates()
	{
		auto device = mGraphicsDevice->GetDevice();
		assert(device != nullptr);

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

			hr = device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());
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

			hr = device->CreateSamplerState(&desc, mSamplerStateLinear.GetAddressOf());
			assert(SUCCEEDED(hr));
		}

		return true;
	}

	bool Renderer::createConstantBuffers()
	{
		auto device = mGraphicsDevice->GetDevice();
		assert(device != nullptr);

		// world, view, projection matrix
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(MatrixBuffer);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		auto hr = device->CreateBuffer(&desc, nullptr, mConstantBufferMatrix.GetAddressOf());
		assert(SUCCEEDED(hr));
	
		return true;
	}

	// ���� ���ó�� ������ �޶�����.
	// ���ĸ�ź���� �����Ϳ� SRV ������ ���Ǿ���.
	bool Renderer::createTextures()
	{
		// ���� ��� : �׽�Ʈ�� ����
		{
			mTexture = std::make_shared<Texture>(L"../Assets/Textures/Choa.jpg");

			// ������ �� Ư�� �� �ֱ�
			{
				mCpuTexture = std::make_shared<Texture>(800, 600);

				auto immediateContext = mGraphicsDevice->GetImmediateContext();

				D3D11_MAPPED_SUBRESOURCE mappedResource;
				immediateContext->Map(mCpuTexture->GetTexture2D(),
					D3D11CalcSubresource(0, 0, 1),
					D3D11_MAP_WRITE_DISCARD,
					0,
					&mappedResource);

				UCHAR* pTexels = (UCHAR*)mappedResource.pData;

				// ���� �ֱ�
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

				immediateContext->Unmap(mCpuTexture->GetTexture2D(), D3D11CalcSubresource(0, 0, 1));
			}
		}

		return true;
	}

	// ���ĸ�ź���� CreateRenderTextures��.
	bool Renderer::createRenderTargets()
	{
		// BackBuffer�� ��� �����ϴ���...
		auto width = mRenderTargetSize.x;
		auto height = mRenderTargetSize.y;

		// �ٽ� ũ�� Ȯ���� �Ѵ�.
		// ũ�Ⱑ ���� �ʴٰ� ������ ���ع����°� �³�?
		if ((width / 4) == 0 || (height / 4) == 0)
		{
			CORE_WARN("{0:d}x{1:d}�� �������� �ʴ� �ػ��Դϴ�.", width, height);
			return false;
		}

		// ��� �׽�Ʈ���̴�.
		{
//			mRenderTargetView = std::make_shared<Texture>(width, height, DXGI_FORMAT_R8G8B8A8_UINT, "Test_RenderTargetView");
		}

		// renderTargets���� �����. Ÿ�Զ����� �����ٰ� �ٽ� ������ �Ѵ�. ���߿� ������ �ʿ��ҵ�?
		if (mRenderTargets[eRenderTargets::Frame_Ldr])
			delete mRenderTargets[eRenderTargets::Frame_Ldr];
		mRenderTargets[eRenderTargets::Frame_Ldr] = new Texture(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

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
			// �� ��Ҹ��� Buffer�� �ٸ��⶧���� inputSlot�� ������ �׾ƾ� �Ѵ�.
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
			mDvFont = new Font;
			result = mDvFont->LoadFromFile("../Assets/Fonts/NanumBarunGothic.ttf");
			assert(result);
		}

		// TextMesh�� Component�� ���� �� ������ �ڵ�
		/*
		{
			m_pTextMesh = new TextMesh;
			result = m_pTextMesh->SetFont(mDvFont);
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
			mPipelineStateColor.pVS = (ID3D11VertexShader*)mShaders[VSTYPE_COLOR].Get();
			mPipelineStateColor.pPS = (ID3D11PixelShader*)mShaders[PSTYPE_COLOR].Get();
			mPipelineStateColor.pDSS = mDepthStencilStates[DSSTYPE_DEFAULT].Get();
			mPipelineStateColor.pRSS = mRasterizerStates[RSSTYPE_CULLBACK_SOLID].Get();
			mPipelineStateColor.pIL = mInputLayouts[ILTYPE_POS_COL].Get();
			mPipelineStateColor.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}

		// Texturing
		{
			mPipelineStateTexturing.pVS = (ID3D11VertexShader*)mShaders[VSTYPE_TEXTURING].Get();
			mPipelineStateTexturing.pPS = (ID3D11PixelShader*)mShaders[PSTYPE_TEXTURING].Get();
			mPipelineStateTexturing.pDSS = mDepthStencilStates[DSSTYPE_DEFAULT].Get();
			mPipelineStateTexturing.pRSS = mRasterizerStates[RSSTYPE_CULLBACK_SOLID].Get();
			mPipelineStateTexturing.pSS = mSamplerState.Get();
			mPipelineStateTexturing.pIL = mInputLayouts[ILTYPE_POS_TEX].Get();
			mPipelineStateTexturing.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}

		// Legacy
		{
			mPipelineStateLegacy.pVS = (ID3D11VertexShader*)mShaders[VSTYPE_LEGACY].Get();
			mPipelineStateLegacy.pPS = (ID3D11PixelShader*)mShaders[PSTYPE_LEGACY].Get();
			mPipelineStateLegacy.pDSS = mDepthStencilStates[DSSTYPE_DEFAULT].Get();
			mPipelineStateLegacy.pRSS = mRasterizerStates[RSSTYPE_CULLBACK_SOLID].Get();
			mPipelineStateLegacy.pSS = mSamplerState.Get();
			mPipelineStateLegacy.pIL = mInputLayouts[ILTYPE_POS_TEX_NOR_TAN].Get();
			mPipelineStateLegacy.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
													 D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}

		// Font
		{
			mPipelineStateFont.pVS = (ID3D11VertexShader*)mShaders[VSTYPE_FONTS].Get();
			mPipelineStateFont.pPS = (ID3D11PixelShader*)mShaders[PSTYPE_FONTS].Get();
			mPipelineStateFont.pDSS = mDepthStencilStates[DSSTYPE_DEFAULT].Get();
			mPipelineStateFont.pRSS = mRasterizerStates[RSSTYPE_CULLBACK_SOLID].Get();
			mPipelineStateFont.pSS = mSamplerStateLinear.Get();
			mPipelineStateFont.pIL = mInputLayouts[ILTYPE_POS_TEX2].Get();
			mPipelineStateFont.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
	}

	// D3D11_INPUT_ELEMENT_DESC�� nullptr�� �ʿ䰡 ���⿡ ������ �޴����� ����.
	bool Renderer::createVertexShader(const std::wstring& filepath, unsigned int shaderType, unsigned int inputLayoutType,
		D3D11_INPUT_ELEMENT_DESC* descs, UINT numElements)
	{
		auto device = mGraphicsDevice->GetDevice();
		assert(device != nullptr);

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
		
		if (FAILED(device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr,
			(ID3D11VertexShader**)mShaders[shaderType].GetAddressOf())))
		{
			CORE_ERROR("Vertex Shader ������ �����Ͽ����ϴ�.");
			return false;
		}
		
		if ((descs != nullptr) && (inputLayoutType != ILTYPE_COUNT))
		{
			if (FAILED(device->CreateInputLayout(descs, numElements, shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(),
				mInputLayouts[inputLayoutType].GetAddressOf())))
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
		auto device = mGraphicsDevice->GetDevice();
		assert(device != nullptr);

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

		if (FAILED(device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr,
			(ID3D11PixelShader**)mShaders[shaderType].GetAddressOf())))
		{
			CORE_ERROR("Pixel Shader ������ �����Ͽ����ϴ�.");
		}

		shaderBuffer->Release(); shaderBuffer = nullptr;

		return true;
	}
}