#include "divepch.h"
#include "GraphicsDevice.h"
#include "Base/Base.h"
#include "Base/Engine.h"

namespace Dive
{
	void GraphicsDevice::Initialize(const WindowData* pData)
	{
		DV_ASSERT(pData != nullptr);

		m_hWnd = pData->hWnd;
		m_Width = pData->Width;
		m_Height = pData->Height;
		m_bVSync = pData->bVSync;
		m_bFullScreen = pData->bFullScreen;

		UINT deviceFlags = 0;

		std::vector<D3D_FEATURE_LEVEL> featureLevels
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.OutputWindow = m_hWnd;
		desc.BufferDesc.Width = m_Width;
		desc.BufferDesc.Height = m_Height;
		desc.Windowed = m_bFullScreen ? FALSE : TRUE;
		desc.BufferCount = m_BackbufferCount;
		desc.BufferDesc.Format = m_Format;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		//desc.BufferDesc.RefreshRate.Numerator = m_bVSync ? refreshRateNumerator : 0;
		//desc.BufferDesc.RefreshRate.Denominator = m_bVSync ? refreshRateDenominator : 1;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = deviceFlags;

		auto createDeviceAndSwapChain = [this, deviceFlags, featureLevels, driverType, desc]()
		{
			return D3D11CreateDeviceAndSwapChain(
				nullptr,									// 역시 임시
				driverType,
				nullptr,
				deviceFlags,
				featureLevels.data(),
				static_cast<UINT>(featureLevels.size()),
				D3D11_SDK_VERSION,
				&desc,
				&m_pSwapChain,
				&m_pDevice,
				nullptr,
				&m_pImmediateContext);
		};

		auto result = createDeviceAndSwapChain();
		if (result == DXGI_ERROR_SDK_COMPONENT_MISSING)
		{
			deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
			result = createDeviceAndSwapChain();
		}

		DV_ASSERT(result == S_OK);

		CreateMainRenderTargetView();
	}

	void GraphicsDevice::Shutdown()
	{
		DV_RELEASE(m_pMainRenderTargetView);
		DV_RELEASE(m_pImmediateContext);
		DV_RELEASE(m_pDevice);
		DV_RELEASE(m_pSwapChain);
	}

	void GraphicsDevice::CreateMainRenderTargetView()
	{
		ID3D11Texture2D* pBackBuffer;
		m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pMainRenderTargetView);
		DV_RELEASE(pBackBuffer);
	}

	void GraphicsDevice::CleanupMainRenderTargetView()
	{
		DV_RELEASE(m_pMainRenderTargetView);
	}

	void GraphicsDevice::ResizeBackBuffer(unsigned int width, unsigned int height)
	{
		DV_ASSERT(m_pSwapChain != nullptr);
		DV_ASSERT(m_pImmediateContext != nullptr);

		m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);

		CleanupMainRenderTargetView();

		if (FAILED(m_pSwapChain->ResizeBuffers(m_BackbufferCount, width, height, m_Format, 0)))
		{
			DV_CORE_WARN("ResizeBuffer를 실패하였습니다.");
			return;
		}

		CreateMainRenderTargetView();

		m_Width = width;
		m_Height = height;
	}

	bool GraphicsDevice::CreateBuffer(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pData, ID3D11Buffer** ppBuffer)
	{
		if (!m_pDevice)
			return false;

		if (!pDesc)
		{
			DV_CORE_WARN("유효하지 않은 매개변수를 전달받았습니다.");
			return false;
		}

		if (FAILED(m_pDevice->CreateBuffer(pDesc, pData, ppBuffer)))
		{
			DV_CORE_WARN("Buffer 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool GraphicsDevice::CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pData, ID3D11Texture2D** ppTexture2D)
	{
		if (!m_pDevice)
			return false;

		if (!pDesc)
		{
			DV_CORE_WARN("유효하지 않은 매개변수를 전달받았습니다.");
			return false;
		}
		
		if (FAILED(m_pDevice->CreateTexture2D(pDesc, pData, ppTexture2D)))
		{
			DV_CORE_WARN("Texture2D 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
	
	bool GraphicsDevice::CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppShaderResourceView)
	{
		if (!m_pDevice)
			return false;

		if (!pResource || !pDesc)
		{
			DV_CORE_WARN("유효하지 않은 매개변수를 전달받았습니다.");
			return false;
		}

		if (FAILED(m_pDevice->CreateShaderResourceView(pResource, pDesc, ppShaderResourceView)))
		{
			DV_CORE_WARN("ShaderResourceView 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
	
	bool GraphicsDevice::CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRenderTargetView)
	{
		if (!m_pDevice)
			return false;

		if (!pResource || !pDesc)
		{
			DV_CORE_WARN("유효하지 않은 매개변수를 전달받았습니다.");
			return false;
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(pResource, pDesc, ppRenderTargetView)))
		{
			DV_CORE_WARN("RenderTargetView 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
	
	bool GraphicsDevice::CreateDepthStencilView(ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView)
	{
		if (!m_pDevice)
			return false;

		if (!pResource || !pDesc)
		{
			DV_CORE_WARN("유효하지 않은 매개변수를 전달받았습니다.");
			return false;
		}

		if (FAILED(m_pDevice->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView)))
		{
			DV_CORE_WARN("DepthStencilView 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}
	bool GraphicsDevice::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* pDesc, ID3D11DepthStencilState** ppDepthStencilState)
	{
		if (!m_pDevice)
			return false;

		if (!pDesc)
		{
			DV_CORE_WARN("유효하지 않은 매개변수를 전달받았습니다.");
			return false;
		}

		if (FAILED(m_pDevice->CreateDepthStencilState(pDesc, ppDepthStencilState)))
		{
			DV_CORE_WARN("DepthStencilState 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool GraphicsDevice::CreateSamplerState(const D3D11_SAMPLER_DESC* pDesc, ID3D11SamplerState** ppSamplerState)
	{
		if (!m_pDevice)
			return false;

		if (!pDesc)
		{
			DV_CORE_WARN("유효하지 않은 매개변수를 전달받았습니다.");
			return false;
		}

		if (FAILED(m_pDevice->CreateSamplerState(pDesc, ppSamplerState)))
		{
			DV_CORE_WARN("SamplerState 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool GraphicsDevice::CreateRasterizerState(const D3D11_RASTERIZER_DESC* pDesc, ID3D11RasterizerState** ppRasterizerState)
	{
		if (!m_pDevice)
			return false;

		if (!pDesc)
		{
			DV_CORE_WARN("유효하지 않은 매개변수를 전달받았습니다.");
			return false;
		}

		if (FAILED(m_pDevice->CreateRasterizerState(pDesc, ppRasterizerState)))
		{
			DV_CORE_WARN("RasterizerState 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	bool GraphicsDevice::CreateVertexShader(const std::string& path, const D3D11_INPUT_ELEMENT_DESC* pDesc, unsigned int numElements, ID3D11VertexShader** ppVertexShader, ID3D11InputLayout** ppInputLayout)
	{
		if (!m_pDevice)
			return false;

		if (!pDesc)
		{
			DV_CORE_WARN("유효하지 않은 매개변수를 전달받았습니다.");
			return false;
		}

		if (!std::filesystem::exists(path))
		{
			DV_CORE_WARN("{:s} 파일이 존재하지 않습니다.", path);
			return false;
		}

		ID3D10Blob* pShaderBuffer = nullptr;
		ID3D10Blob* pShaderError = nullptr;

		DWORD compileFlags = 0;
#if _DEBUG
		compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_PREFER_FLOW_CONTROL;
#else
		compileFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCIMPILE_OPTIMIZATION_LEVEL3;
#endif

		std::wstring filePath(path.begin(), path.end());
		if (FAILED(D3DCompileFromFile(filePath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "mainVS", "vs_5_0", compileFlags, 0, &pShaderBuffer, &pShaderError)))
		{
			if (pShaderError != nullptr)
			{
				DV_CORE_ERROR("{:s}", (char*)pShaderError->GetBufferPointer());
				DV_RELEASE(pShaderError);
			}
			DV_RELEASE(pShaderBuffer);

			return false;
		}

		if (FAILED(m_pDevice->CreateVertexShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr, ppVertexShader)))
		{
			DV_CORE_WARN("VertexShader 생성에 실패하였습니다.");
			return false;
		}
		
		if (FAILED(m_pDevice->CreateInputLayout(pDesc, numElements, pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), ppInputLayout)))
		{
			DV_CORE_WARN("InputLayout 생성에 실패하였습니다.");
			return false;
		}

		DV_RELEASE(pShaderBuffer);

		return true;
	}
	
	bool GraphicsDevice::CreatePixelShader(const std::string& path, ID3D11PixelShader** ppPixelShader)
	{
		if (!m_pDevice)
			return false;

		if (!std::filesystem::exists(path))
		{
			DV_CORE_WARN("{:s} 파일이 존재하지 않습니다.", path);
			return false;
		}

		ID3D10Blob* pShaderBuffer = nullptr;
		ID3D10Blob* pShaderError = nullptr;

		DWORD compileFlags = 0;
#if _DEBUG
		compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_PREFER_FLOW_CONTROL;
#else
		compileFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCIMPILE_OPTIMIZATION_LEVEL3;
#endif

		std::wstring filePath(path.begin(), path.end());
		if (FAILED(D3DCompileFromFile(filePath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "mainPS", "ps_5_0", compileFlags, 0, &pShaderBuffer, &pShaderError)))
		{
			if (pShaderError != nullptr)
			{
				DV_CORE_WARN("{:s}", (char*)pShaderError->GetBufferPointer());
				DV_RELEASE(pShaderError);
			}
			DV_RELEASE(pShaderBuffer);

			return false;
		}

		if (FAILED(m_pDevice->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), nullptr, ppPixelShader)))
		{
			DV_CORE_WARN("PixelShader 생성에 실패하였습니다.");
			return false;
		}

		DV_RELEASE(pShaderBuffer);

		return true;
	}

	bool GraphicsDevice::CreateShader(const std::string& path, const D3D11_INPUT_ELEMENT_DESC* pDesc, unsigned int numElements, ID3D11VertexShader** ppVertexShader, ID3D11InputLayout** ppInputLayout, ID3D11PixelShader** ppPixelShader)
	{
		if (!CreateVertexShader(path, pDesc, numElements, ppVertexShader, ppInputLayout))
		{
			return false;
		}
		else
		{
			if (!CreatePixelShader(path, ppPixelShader))
			{
				return false;
			}
		}

		return true;
	}
}