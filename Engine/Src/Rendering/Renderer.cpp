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
		// ��¿ �� ����. Engine���κ��� WindowData�� ���ͼ� �ʱ�ȭ����.
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
			swapChainDesc.BufferCount							= 1;							// �Ű����� ���
			swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;	// �Ű����� ���
			swapChainDesc.BufferDesc.Width						= m_Width;
			swapChainDesc.BufferDesc.Height						= m_Height;
			swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;							// �Ű����� ���
			swapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;							// �Ű����� ���
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

	// ������κ��� ũ�Ⱑ ����Ǿ��ٴ� �޽���
	void Renderer::OnResize(const WindowResizeEvent * evnt)
	{
	}
}
