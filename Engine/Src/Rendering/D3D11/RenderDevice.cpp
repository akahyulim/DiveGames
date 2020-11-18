#include "DivePch.h"
#include "RenderDevice.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"

namespace Dive
{
	// �ϴ� default adapter�� ����
	RenderDevice::RenderDevice(HWND hWnd, int width, int height, bool windowed)
		: m_hWnd(hWnd), m_Width(width), m_Height(height), m_bWindowed(windowed)
	{
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
		desc.OutputWindow							= m_hWnd;
		desc.BufferDesc.Width						= m_Width;
		desc.BufferDesc.Height						= m_Height;
		desc.Windowed								= m_bWindowed;
		desc.BufferCount							= 1;							// �Ű����� ���
		desc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;	// �Ű����� ���
		desc.BufferDesc.RefreshRate.Denominator		= 1;							// �Ű����� ���
		desc.BufferDesc.RefreshRate.Numerator		= 0;							// �Ű����� ���
		desc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling						= DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count						= 1;
		desc.SampleDesc.Quality						= 0;
		desc.SwapEffect								= DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags									= 0;

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
				&m_SwapChain,
				&m_D3dDevice,
				nullptr,
				&m_ImmediateContext);
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
			return;
		}

		// render target view ����
		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
		{
			CORE_ERROR("");
			return;
		}

		result = m_D3dDevice->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);
		backBuffer->Release();
		if (FAILED(result))
		{
			CORE_ERROR("");
			return;
		}

		m_bInitialize = true;
	}
	
	RenderDevice::~RenderDevice()
	{
		if (m_ImmediateContext)
		{
			m_ImmediateContext->Release();
			m_ImmediateContext = nullptr;
		}

		if (m_D3dDevice)
		{
			m_D3dDevice->Release();
			m_D3dDevice = nullptr;
		}
	}

	bool RenderDevice::Present()
	{
		if (!m_SwapChain)
		{
			CORE_ERROR("");
			return false;
		}

		if (FAILED(m_SwapChain->Present(0, 0)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	void RenderDevice::ResizeBuffer(int width, int height)
	{
		if (width == 0 || height == 0)
		{
			CORE_ERROR("");
			return;
		}

		// ����� �ٸ� ��
		// 1. width, height�� 0���� �־ desc�� �� ������ ����.
		// 2. width, height�� ���޹��� ������ �־ desc�� �� ������ ����.
		// ��, ������ ũ�Ⱑ ������� �ʾҴ�.
		m_SwapChain->ResizeBuffers(
			1,
			width,
			height,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			0);

		m_Width = width;
		m_Height = height;
		CORE_TRACE("Resize Resolution Width: {0:d}, Height: {1:d}", m_Width, m_Height);

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		m_SwapChain->GetDesc(&desc);
		CORE_TRACE("desc Resolution Width: {0:d}, Height: {1:d}", desc.BufferDesc.Width, desc.BufferDesc.Height);

		// back buffer ũ�Ⱑ ������� ������ ���� �����͵� ũ���� ��ȭ�� ����.
		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
		{
			CORE_ERROR("");
			return;
		}
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		backBuffer->GetDesc(&texDesc);
		backBuffer->Release();
		CORE_TRACE("tex Resolution Width: {0:d}, Height: {1:d}", texDesc.Width, texDesc.Height);


		// render target view�� ���� �����ؾ����� ������?
	}
}
