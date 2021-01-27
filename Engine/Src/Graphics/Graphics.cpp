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

		// �� �ָ����.
		g_graphics = this;
	}

	Graphics::~Graphics()
	{
		SAFE_DELETE(m_deviceAndSwapChain);

		CORE_TRACE("Call Graphics's Destructor ======================");
	}

	// �� ������ input�� ó���Ѵ�?
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

		// ���ĸ�ź�� �� �κ� ���� CommandList���� ó���Ѵ�.
		// ��, Begin, EndFrame ���� �Լ��� ����.
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
		// �� �ָ��ϴ�.
		return (m_window != nullptr && m_deviceAndSwapChain != nullptr);
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

	// �����Լ��� ���⿡�� �̸��� �ָ��ϴ�.
	// �� ���¿��� �ִ�ũ�� ȭ���� ���� �� ����.
	// ����� �Է¹޾� �����Ϸ��� ���ŷӴ�.
	bool Graphics::SetMode(int width, int height, bool fullScreen, bool borderless, bool vSync)
	{
		DisplayMode mode;
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

	void Graphics::ResizeTextures(unsigned int width, unsigned int height)
	{
		// �ϴ� �����ϴ� ũ������ Ȯ���Ѵ�.

	//	if (m_width == width && m_height == height)
		if(m_textureSize.x == width && m_textureSize.y == height)
			return;

		// ũ�⸦ �����Ѵ�.
		m_textureSize = DirectX::XMUINT2(width, height);

		// �ؽ��ĸ� ���Ӱ� �����Ѵ�.
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

	bool Graphics::createDisplay(int width, int height, DisplayMode displayMode)
	{
		// ������ ����
		if (!m_window->Create(
			width,
			height,
			displayMode.screenMode == eScreenMode::FullScreen,
			displayMode.screenMode == eScreenMode::Windowed
		))
		{
			return false;
		}

		// �ϴ� ũ�⸦ �����ϰ� �Ѵ�.
		auto clientWidth = m_window->GetClientRectWidth();
		auto clientHeight = m_window->GetClientRectHeight();
		m_textureSize.x = clientWidth;
		m_textureSize.y = clientHeight;
		m_displayMode = displayMode;

		m_deviceAndSwapChain = new DeviceAndSwapChain(m_context, m_window.get()->GetHandle(), clientWidth, clientHeight,
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

	// �뵵�� �� �� ��Ȯ�� �ؾ� �Ѵ�.
	void Graphics::adjustDisplayMode(int & width, int & height, DisplayMode & screenMode)
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
	}

	bool Graphics::createDepthStencilStates()
	{
		m_depthStencilStateEnabled	= std::make_shared<DepthStencilState>(m_context, TRUE, D3D11_COMPARISON_LESS);
		m_depthStencilStateDisabled = std::make_shared<DepthStencilState>(m_context, FALSE, D3D11_COMPARISON_LESS);

		if (!m_depthStencilStateEnabled || !m_depthStencilStateDisabled)
		{
			CORE_ERROR("Fail to create depthstencil states.");
			return false;
		}

		return true;
	}

	bool Graphics::createBlendStates()
	{
		m_blendStateEnable		= std::make_shared<BlendState>(m_context, TRUE);
		m_blendStateDisable		= std::make_shared<BlendState>(m_context, FALSE);
		m_blendStateColorAdd	= std::make_shared<BlendState>(m_context, TRUE, D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);

		if (!m_blendStateEnable || !m_blendStateDisable || !m_blendStateColorAdd)
		{
			CORE_ERROR("Fail to create beldn states.");
			return false;
		}

		return true;
	}

	bool Graphics::createRasterizerStates()
	{
		m_rasterizeStateCullBackSolid		= std::make_shared<RasterizerState>(m_context, D3D11_CULL_BACK, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
		m_rasterizeStateCullFrontSolid		= std::make_shared<RasterizerState>(m_context, D3D11_CULL_FRONT, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);
		m_rasterizeStateCullNoneSolid		= std::make_shared<RasterizerState>(m_context, D3D11_CULL_NONE, D3D11_FILL_SOLID, TRUE, FALSE, FALSE, FALSE);

		m_rasterizeStateCullBackWireFrame	= std::make_shared<RasterizerState>(m_context, D3D11_CULL_BACK, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
		m_rasterizeStateCullFrontWireFrame	= std::make_shared<RasterizerState>(m_context, D3D11_CULL_FRONT, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);
		m_rasterizeStateCullNoneWireFrame	= std::make_shared<RasterizerState>(m_context, D3D11_CULL_NONE, D3D11_FILL_WIREFRAME, TRUE, FALSE, FALSE, TRUE);

		if (!m_rasterizeStateCullBackSolid || !m_rasterizeStateCullFrontSolid || !m_rasterizeStateCullNoneSolid
			|| !m_rasterizeStateCullBackWireFrame || !m_rasterizeStateCullFrontWireFrame || !m_rasterizeStateCullNoneWireFrame)
		{
			CORE_ERROR("Fail to create rasterizer states.");
			return false;
		}

		return true;
	}

	bool Graphics::createSampels()
	{
		m_samplerLinear				= std::make_shared<Sampler>(m_context, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

		m_samplerCompareDepth		= std::make_shared<Sampler>(m_context, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_GREATER_EQUAL);
		m_samplerPointClamp			= std::make_shared<Sampler>(m_context, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_ALWAYS);
		m_samplerBilinearClamp		= std::make_shared<Sampler>(m_context, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP);
		m_samplerBilinearWrap		= std::make_shared<Sampler>(m_context, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP);
		m_samplerTrilinearClamp		= std::make_shared<Sampler>(m_context, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
		m_samplerAnisotropicWrap	= std::make_shared<Sampler>(m_context, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, 16);

		if (!m_samplerLinear || !m_samplerCompareDepth || !m_samplerPointClamp | !m_samplerBilinearClamp
			|| !m_samplerBilinearWrap || !m_samplerTrilinearClamp || !m_samplerAnisotropicWrap)
		{
			CORE_ERROR("fail to create samplers.");
			return false;
		}

		return true;
	}
	
	bool Graphics::createShaders()
	{
		return false;
	}

	// ����ϴ� ��� Texture�� ������ ũ��� �����Ѵ�.
	// �̴� ResizeTextures()�� ���� ���ο� ũ��� ���ŵ� �� �ִ�.
	bool Graphics::createTextures()
	{
		// �����Ϸ��� ���� �� ���� ����ų� ����Ʈ �����͸� ��� �Ѵ�.
		m_editorView = new Texture2D(m_context, "EditorView");
		m_editorView->CreateRenderTarget(m_textureSize.x, m_textureSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT);

		m_depthStencil = new Texture2D(m_context, "DepthStencil");
		m_depthStencil->CreateDepthStencil(m_textureSize.x, m_textureSize.y, true);

		if (!m_editorView || !m_depthStencil)
		{
			return false;
		}

		return true;
	}

	bool Graphics::createBaseShader()
	{
		m_baseShader = new Shader(m_context);
		if (!m_baseShader->CreateShader(eVertexType::PositionUvNormalTangent, L"../Assets/Shaders/baseShader.hlsl"))
			return false;

		return true;
	}

	// ������ ������ �� update�� ���Ž��Ѿ� �Ѵ�.
	bool Graphics::createConstantBuffer()
	{
		m_constantBuffer = new ConstantBuffer(m_context);
		if (!m_constantBuffer->Create<MatrixBuffer>())
			return false;

		// view
		DirectX::XMMATRIX view;
//		{
			DirectX::XMFLOAT3 pos, lookAt, up;
			pos.x = 0.0f; pos.y = 0.0f; pos.z = -5.0f;
			lookAt.x = 0.0f; lookAt.y = 0.0f; lookAt.z = 0.0f;
			up.x = 0.0f, up.y = 1.0f; up.z = 0.0f;
			DirectX::XMVECTOR vecPos, vecLookAt, vecUp;
			vecPos = DirectX::XMLoadFloat3(&pos);
			vecLookAt = DirectX::XMLoadFloat3(&lookAt);
			vecUp = DirectX::XMLoadFloat3(&up);
			view = DirectX::XMMatrixLookAtLH(vecPos, vecLookAt, vecUp);
//		}

		// proj
		DirectX::XMMATRIX proj;
//		{
			float fieldOfView = 3.141592654f / 4.0f;
			float screenAspect = (float)m_textureSize.x / (float)m_textureSize.y;
			proj = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f);
//		}

		auto ptr = static_cast<MatrixBuffer*>(m_constantBuffer->Map());
		DirectX::XMStoreFloat4x4(&(ptr->world), DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()));
		//DirectX::XMStoreFloat4x4(&(ptr->view), DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()));
		DirectX::XMStoreFloat4x4(&(ptr->view), DirectX::XMMatrixTranspose(view));
		DirectX::XMStoreFloat4x4(&(ptr->proj), DirectX::XMMatrixTranspose(proj));

		if (!m_constantBuffer->Unmap())
			return false;

		return true;
	}
}
