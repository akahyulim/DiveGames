#include "DivePch.h"
#include "Graphics.h"
#include "Window.h"
#include "Core/Log.h"


namespace Dive
{
	bool Graphics::createRHI()
	{
		// ������ ���� ���θ� Ȯ���ؾ� �Ѵ�.
		// �׷��� shared_ptr�̶� ��Ŭ�� ������ Ȯ���� �ȵȴ�.

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
		desc.BufferDesc.Width					= m_window->GetClientRectWidth();
		desc.BufferDesc.Height					= m_window->GetClientRectHeight();
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
}