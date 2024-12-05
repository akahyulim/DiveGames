#include "stdafx.h"
#include "Graphics.h"
#include "core/CoreDefs.h"

namespace Dive
{
	constexpr uint32_t DV_BUFFER_COUNT = 2;
	constexpr uint32_t DV_REFRESHRATE_NUMERATOR = 60;
	constexpr uint32_t DV_REFRESHRATE_DENOMINATOR = 1;
	constexpr DXGI_FORMAT DV_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr BOOL DV_WINDOWED = TRUE;

	Graphics::Graphics()
		: m_pSwapChain(nullptr)
		, m_pDevice(nullptr)
		, m_pDeviceContext(nullptr)
		, m_pBackbufferView(nullptr)
		, m_pBackbufferDepthTex(nullptr)
		, m_pBackbufferDepthView(nullptr)
		, m_Resolution(8, 8)
		, m_bVSync(false)
	{
	}

	Graphics::~Graphics()
	{
		Shutdown();
	}

	bool Graphics::Initialize(uint32_t width, uint32_t height, HWND hWnd)
	{
		m_Resolution = { width, height };

		if (FAILED(::D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr, //&featureLevel,//nullptr,
			0, //1, //0,
			D3D11_SDK_VERSION,
			&m_pDevice,
			nullptr,
			&m_pDeviceContext)))
		{
			DV_RELEASE(m_pDevice);
			DV_RELEASE(m_pDeviceContext);
			DV_LOG(Graphics, err, "D3D11Device & D3D11DeviceContext ���� ����");
			return false;
		}

		IDXGIDevice* pDxgiDevice{};
		m_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter{};
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory{};
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		DXGI_SWAP_CHAIN_DESC desc{};
		desc.BufferCount = DV_BUFFER_COUNT;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Format = DV_FORMAT;
		desc.BufferDesc.RefreshRate.Denominator = DV_REFRESHRATE_DENOMINATOR;
		desc.BufferDesc.RefreshRate.Numerator = DV_REFRESHRATE_NUMERATOR;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// ��Ƽ ���ø� off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = DV_WINDOWED;
		desc.OutputWindow = hWnd;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek���� 0�̰� �ٸ� ���� ������ ���� �ִ�...

		if (FAILED(pDxgiFactory->CreateSwapChain(m_pDevice, &desc, &m_pSwapChain)))
		{
			DV_RELEASE(m_pSwapChain);
			DV_LOG(Graphics, err, "D3D11SwapChain ���� ����");
			return false;
		}

		// ���� ������ �߻��ϸ� ��������
		DV_RELEASE(pDxgiFactory);
		DV_RELEASE(pDxgiAdapter);
		DV_RELEASE(pDxgiDevice);

		if (!updateBackbuffer())
			return false;


		DV_SUBSCRIBE_EVENT(eEventType::WindowResized, DV_EVENT_HANDLER_DATA(OnWindowResized));

		DV_LOG(Graphics, trace, "�ʱ�ȭ ����");

		return true;
	}

	void Graphics::Shutdown()
	{
		DV_RELEASE(m_pBackbufferDepthView);
		DV_RELEASE(m_pBackbufferDepthTex);
		DV_RELEASE(m_pBackbufferView);

		DV_RELEASE(m_pDeviceContext);
		DV_RELEASE(m_pDevice);
		DV_RELEASE(m_pSwapChain);

		DV_LOG(Graphics, trace, "�˴ٿ� ����");
	}

	void Graphics::ChangeResolution(uint32_t width, uint32_t height)
	{
		if (m_Resolution.x == width && m_Resolution.y == height)
			return;

		m_Resolution = { width, height };

		DV_RELEASE(m_pBackbufferView);
		if (FAILED(m_pSwapChain->ResizeBuffers(DV_BUFFER_COUNT, width, height, DV_FORMAT, 0)))
		{
			DV_LOG(Graphics, err, "�ĸ� ���� ũ�� {0:d}x{1:d} ���濡 �����Ͽ����ϴ�.", width, height);
			return;
		}

		DXGI_MODE_DESC desc{};
		desc.Width = width;
		desc.Height = height;
		desc.RefreshRate.Numerator = DV_REFRESHRATE_NUMERATOR;
		desc.RefreshRate.Denominator = DV_REFRESHRATE_DENOMINATOR;
		desc.Format = DV_FORMAT;

		if (FAILED(m_pSwapChain->ResizeTarget(&desc)))
		{
			DV_LOG(Graphics, err, "�ػ� {0:d}x{1:d} ���濡 �����Ͽ����ϴ�.", width, height);
			return;
		}

		updateBackbuffer();
	}

	void Graphics::OnWindowResized(EventData data)
	{
		if (auto pairData = std::get_if<void*>(&data)) 
		{
			auto actualData = static_cast<std::pair<uint32_t, uint32_t>*>(*pairData);
			ChangeResolution(actualData->first, actualData->second);
		}
	}

	bool Graphics::updateBackbuffer()
	{
		DV_RELEASE(m_pBackbufferView);
		DV_RELEASE(m_pBackbufferDepthTex);
		DV_RELEASE(m_pBackbufferDepthView);

		ID3D11Texture2D* pBackbufferTexture{};
		if (FAILED(m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_LOG(Graphics, err, "�ĸ���� �ؽ��� ȹ�� ����");
			return false;
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), nullptr, &m_pBackbufferView)))
		{
			DV_RELEASE(m_pBackbufferView);
			DV_LOG(Graphics, err, "�ĸ���� ����Ÿ�ٺ� ���� ����");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		DXGI_SWAP_CHAIN_DESC desc{};
		m_pSwapChain->GetDesc(&desc);

		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = desc.BufferDesc.Width;
		texDesc.Height = desc.BufferDesc.Height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;// static_cast<UINT>(screenParamm_.multiSample_);
		texDesc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(texDesc.Format, screenParamm_.multiSample_);
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pBackbufferDepthTex)))
		{
			DV_LOG(Graphics, err, "�ĸ���� ���� ���ٽ� �ؽ��� ���� ����");
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pDevice->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(m_pBackbufferDepthTex),
			&viewDesc,//nullptr,		// urho�� nullptr�� �����ߴ�. ���ó� sampler ������ �ذ���� �ʾҴ�.
			&m_pBackbufferDepthView)))
		{
			DV_LOG(Graphics, err, "�ĸ���� ���� ���ٽ� �� ���� ����");
			return false;
		}

		DV_LOG(Graphics, info, "������Ʈ�� �ĸ���� ũ�� - {0:d}x{1:d}", desc.BufferDesc.Width, desc.BufferDesc.Height);

		return true;
	}
}