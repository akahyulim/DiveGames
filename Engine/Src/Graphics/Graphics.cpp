#include "DivePch.h"
#include "Graphics.h"
#include "Window.h"
#include "DeviceAndSwapChain.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"
#include "D3D11/DepthStencilState.h"
#include "D3D11/BlendState.h"
#include "D3D11/RasterizerState.h"
#include "D3D11/Sampler.h"
#include "D3D11/Shader.h"
#include "D3D11//Texture2D.h"
#include "D3D11/ConstantBuffer.h"


namespace Dive
{
	Graphics* g_graphics = nullptr;

	Graphics::Graphics(Context* context)
		: Object(context),
		m_deviceAndSwapChain(nullptr)

	{
		m_window = std::make_shared<Window>();
		assert(m_window);

		// 좀 애매허다.
		g_graphics = this;
	}

	Graphics::~Graphics()
	{
		CORE_TRACE("Call Graphics's Destructor ======================");
	}

	// 이 곳에서 input을 처리한다?
	LRESULT Graphics::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if ((msg == WM_SIZE) || (msg == WM_DISPLAYCHANGE))
		{
			ResizeResolution();
		}
		
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	bool Graphics::BeginFrame()
	{
		if (!IsInitialized())
			return false;

		if (!m_window->Run())
		{
			m_window->Destroy();
			return false;
		}

		// 스파르탄은 이 부분 역시 CommandList에서 처리한다.
		// 즉, Begin, EndFrame 같은 함수가 없다.
		// render target clear
		auto immediateContext = m_deviceAndSwapChain->GetImmediateContext();
		auto renderTargetView = m_deviceAndSwapChain->GetRenderTargetView();
		float clear_color[4]{ 0.1f, 0.1f, 0.1f, 1.0f };
		immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);// m_depthStencil->GetDepthStencilView());
		immediateContext->ClearRenderTargetView(renderTargetView, (FLOAT*)(&clear_color));
		//immediateContext->ClearDepthStencilView(m_depthStencil->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		m_deviceAndSwapChain->Present();
	}

	bool Graphics::IsInitialized() const
	{
		// 좀 애매하다.
		return (m_window && m_deviceAndSwapChain && m_deviceAndSwapChain->IsInitialized());
	}

	DirectX::XMUINT2 Graphics::GetResolution() const
	{
		if(!IsInitialized())
			return DirectX::XMUINT2(0 , 0);

		return m_deviceAndSwapChain->GetSwapChainSize();
	}

	void Graphics::SetResolution(DirectX::XMUINT2 size, bool force)
	{
		if (!IsInitialized())
			return;

		m_deviceAndSwapChain->ResizeTarget(size, force);
	}

	void Graphics::ResizeResolution(DirectX::XMUINT2 size)
	{
		if (!IsInitialized())
			return;

		m_deviceAndSwapChain->ResizeBuffer(size);
	}

	// 생성함수로 보기에는 이름이 애매하다.
	// 이 상태에서 최대크기 화면을 만들 수 없다.
	// 사이즈를 입력받아 적용하려면 번거롭다.
	bool Graphics::SetMode(int width, int height, bool fullScreen, bool borderless, bool vSync)
	{
		DISPLAY_MODE mode;
		mode.fullScreen = fullScreen;
		mode.borderless = borderless;
		mode.vSync = vSync;

		mode.screenMode = fullScreen ? eScreenMode::FullScreen : (borderless ? eScreenMode::Borderless : eScreenMode::Windowed);
	
		return createDisplay(width, height, mode);
	}

	bool Graphics::SetWindowSubclassing(LONG_PTR newProc)
	{
		if (!m_window)
			return false;
		
		return m_window->ChangeWndProc(newProc);
	}

	void Graphics::ResizeTextures(const DirectX::XMUINT2& size)
	{
		// 일단 지원하는 크기인지 확인한다.

		if(m_textureSize.x == size.x && m_textureSize.y == size.y)
			return;

		m_textureSize = size;

		createTextures();

		CORE_INFO("Change resolution {0:d} x {1:d}", m_textureSize.x, m_textureSize.y);

		return;
	}

	ID3D11Device * Graphics::GetRHIDevice() const
	{
		return IsInitialized() ? m_deviceAndSwapChain->GetDevice() : nullptr;
	}

	ID3D11DeviceContext * Graphics::GetRHIContext() const
	{
		return IsInitialized() ? m_deviceAndSwapChain->GetImmediateContext() : nullptr;
	}

	RasterizerState * Graphics::GetRasterizerState(eRasterizerState state)
	{
		switch (state)
		{
		case eRasterizerState::CullBackSolid:		return m_rasterizeStateCullBackSolid.get();
		case eRasterizerState::CullFrontSolid:		return m_rasterizeStateCullFrontSolid.get();
		case eRasterizerState::CullNoneSolid:		return m_rasterizeStateCullNoneSolid.get();
		case eRasterizerState::CullBackWireFrame:	return m_rasterizeStateCullBackWireFrame.get();
		case eRasterizerState::CullFrontWireFrame:	return m_rasterizeStateCullFrontWireFrame.get();
		case eRasterizerState::CullNoneWireFrame:	return m_rasterizeStateCullNoneWireFrame.get();

		default: return nullptr;
		}
	}

	BlendState * Graphics::GetBlendState(eBlendState state)
	{
		switch (state)
		{
		case eBlendState::Enabled:		return m_blendStateEnable.get();
		case eBlendState::Disabled:		return m_blendStateDisable.get(); 
		case eBlendState::ColorAdded:	return m_blendStateColorAdd.get();

		default: return nullptr;
		}
	}

	DepthStencilState * Graphics::GetDepthStencilState(bool enabled)
	{
		return enabled ? m_depthStencilStateEnabled.get() : m_depthStencilStateDisabled.get();
	}

	Sampler * Graphics::GetSampler(eSamplerType type)
	{
		switch (type)
		{
		case eSamplerType::Linear:			return m_samplerLinear.get();
		case eSamplerType::CompareDepth:	return m_samplerCompareDepth.get();
		case eSamplerType::PointClamp:		return m_samplerPointClamp.get();
		case eSamplerType::BilinearClamp:	return m_samplerBilinearClamp.get();
		case eSamplerType::BilinearWrap:	return m_samplerBilinearWrap.get();
		case eSamplerType::TrilinearClamp:	return m_samplerTrilinearClamp.get();
		case eSamplerType::AnisotropicWrap: return m_samplerAnisotropicWrap.get();
	
		default: return nullptr;
		}
	}

	bool Graphics::createDisplay(int width, int height, DISPLAY_MODE displayMode)
	{
		// 윈도우 생성
		if (!m_window->Create(
			width,
			height,
			displayMode.screenMode == eScreenMode::FullScreen,
			displayMode.screenMode == eScreenMode::Windowed
		))
		{
			return false;
		}

		// 일단 크기를 동일하게 한다.
		auto clientWidth = m_window->GetClientRectWidth();
		auto clientHeight = m_window->GetClientRectHeight();
		m_textureSize.x = clientWidth;
		m_textureSize.y = clientHeight;
		m_displayMode = displayMode;

		m_deviceAndSwapChain = std::make_shared<DeviceAndSwapChain>(m_context, m_window.get()->GetHandle(), clientWidth, clientHeight,
			displayMode.screenMode == eScreenMode::Windowed ? TRUE : FALSE);
		if (!m_deviceAndSwapChain->IsInitialized())
			return false;

		CORE_TRACE("Window Size: {0:d} x {1:d}", m_window->GetWidth(), m_window->GetHeight());
		CORE_TRACE("Client Rect Size: {0:d} x {1:d}", m_textureSize.x, m_textureSize.y);

		// create default gpu resources
		if (!createDepthStencilStates())
			return false;
		if (!createBlendStates())
			return false;
		if (!createRasterizerStates())
			return false;
		if (!createSampels())
			return false;

		if(!createBaseShader())
			return false;

		if (!createTextures())
			return false;

		if (!createConstantBuffer())
			return false;

		return true;
	}

	// 용도를 좀 더 명확히 해야 한다.
	void Graphics::adjustDisplayMode(int & width, int & height, DISPLAY_MODE & screenMode)
	{
		/*
		HRESULT hResult;

		IDXGIFactory* factory = nullptr;
		hResult = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if(FAILED(hResult))
		{
			CORE_ERROR("");
			return;
		}

		IDXGIAdapter* adapter = nullptr;
		hResult = factory->EnumAdapters(0, &adapter);
		if (FAILED(hResult))
		{

		}

		DXGI_ADAPTER_DESC adapterDesc;
		hResult = adapter->GetDesc(&adapterDesc);
		if (FAILED(hResult))
		{

		} 
		m_vgaInfo.name = adapterDesc.Description;
		m_vgaInfo.videoMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		
		IDXGIOutput* output = nullptr;
		hResult = adapter->EnumOutputs(0, &output);
		if (FAILED(hResult))
		{

		}

		unsigned int numModes;
		hResult = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if (FAILED(hResult))
		{

		}

		DXGI_MODE_DESC* modeList = new DXGI_MODE_DESC[numModes];
		hResult = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, modeList);
		if (FAILED(hResult))
		{

		}

		// 결국 해상도와 refresh rate를 얻어오는 용도다.
		// 그런데 이는 전체화면일 때 의미가 있다.
		m_adapterInfos.reserve(numModes);
		for (int i = 0; i != numModes; i++)
		{
			m_adapterInfos.emplace_back(modeList[i].Width, modeList[i].Height, modeList[i].RefreshRate.Numerator, modeList[i].RefreshRate.Denominator);
			//if (modeList[i].Width == static_cast<unsigned int>(width) && modeList[i].Height == static_cast<unsigned int>(height))
			{

			}

			CORE_TRACE("{0:d} x {1:d} : {2:d} / {3:d}", modeList[i].Width, modeList[i].Height, modeList[i].RefreshRate.Numerator, modeList[i].RefreshRate.Denominator);
		}
		delete[] modeList;

		SAFE_RELEASE(output);
		SAFE_RELEASE(adapter);
		SAFE_RELEASE(factory);
		*/
	}
}
