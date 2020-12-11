#include "DivePch.h"
#include "Graphics.h"
#include "Window.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"
#include "D3D11/DepthStencilState.h"
#include "D3D11/VertexBuffer.h"


namespace Dive
{
	Graphics::Graphics(Context* context)
		: Object(context)

	{
		m_window = std::make_shared<Window>();
		assert(m_window);
	}

	Graphics::~Graphics()
	{
		SAFE_RELEASE(m_immediateContext);
		SAFE_RELEASE(m_device);
		SAFE_RELEASE(m_swapChain);

		m_window->Destroy();
		m_window.reset();

		CORE_TRACE("Call Graphics's Destructor ======================");
	}

	bool Graphics::BeginFrame()
	{
		if (!IsInitialized() || !m_window)
			return false;

		if (!m_window->Run())
			return false;

		// render target clear
		float clear_color[4]{ 0.1f, 0.1f, 0.1f, 1.0f };
		m_immediateContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
		m_immediateContext->ClearRenderTargetView(m_renderTargetView, (float*)&clear_color);

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!m_swapChain)
		{
			CORE_ERROR("");
			return;
		}

		UINT syncInterval = 0;
		if (m_bVSync)
			syncInterval = 1;
		if (FAILED(m_swapChain->Present(syncInterval, 0)))
		{
			CORE_ERROR("");
			return;
		}
	}

	bool Graphics::IsInitialized()
	{
		return ( m_window != nullptr && m_device != nullptr);
	}

	bool Graphics::SetMode(int width, int height, bool fullScreen, bool borderless, bool resizable, bool vSync)
	{
		ScreenMode mode;
		mode.fullScreen = fullScreen;
		mode.borderless = borderless;
		mode.resizable = resizable;
		mode.vSync = vSync;

		bool maximize = (width == 0 || height == 0) && (!fullScreen) && (!borderless) && (resizable);

		return createGraphicsDisplay(width, height, mode, maximize);
	}

	bool Graphics::SetVertexBuffer(std::shared_ptr<VertexBuffer> vertexBuffer)
	{
		// ���� ���⿡ weak_ptr�� ������ �ʿ䰡 ������?
		// �������� ��� ������ ������ �ϱ����� ���� ���۸� ���ܵδ� �뵵�� ����ߴ�.
		// �׷��� urho3d�� �� �ٸ� ����̴�. �� ������߰ڴ�.
		if (!vertexBuffer)
			return false;

		m_vertexBuffer = vertexBuffer;

		// pipeline�� ���
		//m_immediateContext->IASetVertexBuffers()

		return true;
	}

	bool Graphics::createGraphicsDisplay(int width, int height, ScreenMode screenMode, bool maximize)
	{
		adjustScreenMode(width, height, screenMode);

		// ������ ����
		if (!m_window->Create(width, height))
			return false;
		m_window->Show();

		if (maximize)
		{
			// �ִ�ȭ
			// �������� SW�� �����ϴ� �Լ��̴�.
			// ��, ������ �� �ǹ̸� ������.

			// �ִ�ȭ�� �ƴϴ��� Ŭ���̾�Ʈ ũ�⸦ �����ؾ� �ϴµ�...
			// => adjust���� �����߾�� �Ѵ�. �׷��� ������ ���� ���̴�...
			// => ã�ƺ��� Adjust ScreenMode�� Window�� ������ �ִ�.
			//width = m_window->GetClientRectWidth();
			//height = m_window->GetClientRectHeight();
		}

		m_width = m_window->GetClientRectWidth();
		m_height = m_window->GetClientRectHeight();
		m_screenMode = screenMode;

		// d3d11 ����
		if (!createRHI())
			return false;

		CORE_TRACE("Window Size: {0:d} x {1:d}", width, height);
		CORE_TRACE("Client Rect Size: {0:d} x {1:d}", m_width, m_height);

		return true;
	}

	// �뵵�� �� �� ��Ȯ�� �ؾ� �Ѵ�.
	void Graphics::adjustScreenMode(int & width, int & height, ScreenMode & screenMode)
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

		// �ᱹ �ػ󵵿� refresh rate�� ������ �뵵��.
		// �׷��� �̴� ��üȭ���� �� �ǹ̰� �ִ�.
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

		// ũ�⸦ �����Ѵ�.
		if (width == 0 || height == 0)
		{
			if (screenMode.fullScreen || screenMode.borderless)
			{
				// �ִ�ȭ�Ѵ�.
			}
			else
			{
				width = 1024;
				height = 768;
			}
		}
	}

	bool Graphics::createRHI()
	{
		if (!m_window)
			return false;

		m_width = m_window->GetClientRectWidth();
		m_height = m_window->GetClientRectHeight();

		UINT deviceFlags = 0;
#ifdef _DEBUG
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
		desc.OutputWindow						= m_window->GetHandle();
		desc.BufferDesc.Width					= m_width;
		desc.BufferDesc.Height					= m_height;
		desc.Windowed							= m_window->IsFullScreen() ? false : true;
		desc.BufferCount						= 1;							// �Ű����� ���
		desc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;	// �Ű����� ���
		desc.BufferDesc.RefreshRate.Denominator = 1;							// �Ű����� ���
		desc.BufferDesc.RefreshRate.Numerator	= 0;							// �Ű����� ���
		desc.BufferDesc.ScanlineOrdering		= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count					= 1;
		desc.SampleDesc.Quality					= 0;
		desc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags								= 0;

		auto createDevice = [this, deviceFlags, featureLevels, driverType, desc]()
		{
			return D3D11CreateDeviceAndSwapChain(
				nullptr,									// ���� �ӽ�
				driverType,
				nullptr,
				deviceFlags,
				featureLevels.data(),
				static_cast<UINT>(featureLevels.size()),
				D3D11_SDK_VERSION,
				&desc,
				&m_swapChain,
				&m_device,
				nullptr,
				&m_immediateContext);
		};

		auto result = createDevice();
		if (result == DXGI_ERROR_SDK_COMPONENT_MISSING)
		{
			CORE_WARN("");
			deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
			result = createDevice();
		}

		if (FAILED(result))
		{
			CORE_ERROR("");
			return false;
		}

		// render target view ����
		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
		{
			CORE_ERROR("");
			return false;
		}

		result = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
		backBuffer->Release();
		if (FAILED(result))
		{
			CORE_ERROR("");
			return false;
		}

		CORE_TRACE("Created d3d11 rhi.");

		return true;
	}

	bool Graphics::depthStencilStates()
	{
		m_depthStencilStateEnabled = std::make_shared<DepthStencilState>(m_context, true, D3D11_COMPARISON_LESS);
		m_depthStencilStateDisabled = std::make_shared<DepthStencilState>(m_context, false, D3D11_COMPARISON_LESS);

		if (!m_depthStencilStateEnabled || !m_depthStencilStateDisabled)
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
}
