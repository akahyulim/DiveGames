#include "DivePch.h"
#include "Renderer.h"
#include "Core/Engine.h"
#include "Core/SystemManager.h"
#include "Core/Log.h"
#include "Events/WindowEvent.h"

namespace Dive
{
	Renderer::Renderer(SystemManager* manager)
		: ISystem(manager)
	{
		EventSystem::GetInstance().Subscribe(this, &Renderer::OnResize);
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::Initialize()
	{
		// 어쩔 수 없다. Engine으로부터 WindowData를 얻어와서 초기화하자.
		const auto& wndData = m_Manager->GetEngine()->GetWindowData();
		m_hWnd		= wndData.hWnd;
		m_Width		= wndData.width;
		m_Height	= wndData.height;

		// IDXGI settings

		// create Device & SwapChain
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

			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			swapChainDesc.BufferCount							= 1;							// 매개변수 대상
			swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;	// 매개변수 대상
			swapChainDesc.BufferDesc.Width						= m_Width;
			swapChainDesc.BufferDesc.Height						= m_Height;
			swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;							// 매개변수 대상
			swapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;							// 매개변수 대상
			swapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
			swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.OutputWindow							= m_hWnd;
			swapChainDesc.Windowed								= true;
			swapChainDesc.SampleDesc.Count						= 1;
			swapChainDesc.SampleDesc.Quality					= 0;
			swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
			swapChainDesc.Flags									= 0;
		}

		// create RenderTargetView
		{

		}
		
		return true;
	}
	
	void Renderer::Update()
	{
	}

	void Renderer::Present()
	{
	}

	// 윈도우로부터 크기가 변경되었다는 메시지
	void Renderer::OnResize(const WindowResizeEvent * evnt)
	{
	}
}
