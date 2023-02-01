#include "divepch.h"
#include "Graphics.h"
#include "GraphicsEvents.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "InputLayout.h"
#include "Texture2D.h"
#include "RenderTexture.h"
#include "Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Material.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Core/EventSystem.h"
#include "Resource/ResourceCache.h"
#include "IO/Log.h"

namespace Dive
{
	bool Graphics::CreateDeviceAndSwapChain(uint32_t width, uint32_t height)
	{
		/*
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		if (FAILED(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			&featureLevel,//nullptr,
			1, //0,
			D3D11_SDK_VERSION,
			&m_pDevice,
			nullptr,
			&m_pDeviceContext)))
		{
			DV_LOG_ENGINE_ERROR("Graphics::createDeviceAndSwapChain - D3D11 ��ġ ������ �����Ͽ����ϴ�.");
			return false;
		}

		// ��Ƽ ���� ���� üũ?

		IDXGIDevice* pDxgiDevice = nullptr;
		m_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter = nullptr;
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory = nullptr;
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		// ���� �ػ󵵿��� ����ȭ�� ���÷��� ����Ʈ ����
		// �ϴ� �ǳʶڴ�.

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 1;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB???
		desc.BufferDesc.RefreshRate.Denominator = 1;			// ���� ����(vsync�� ���� �޶�����?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// ��Ƽ ���ø� off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = IsFullScreen() ? FALSE : TRUE;	// orho�� TRUE �����̴�. ��üȭ���� ���⿡�� �����ȴ�.
		desc.OutputWindow = m_WindowHandle;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek���� 0�̰� �ٸ� ���� ������ ���� �ִ�...

		if (FAILED(pDxgiFactory->CreateSwapChain(m_pDevice, &desc, &m_pSwapChain)))
		{
			DV_LOG_ENGINE_ERROR("Graphics::createDeviceAndSwapChain - D3D11 ����ü�� ������ �����Ͽ����ϴ�.");
			return false;
		}
		*/

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
		desc.OutputWindow = m_WindowHandle;
		desc.BufferDesc.Width = m_Width;
		desc.BufferDesc.Height = m_Height;
		desc.Windowed = IsFullScreen() ? FALSE : TRUE;
		desc.BufferCount = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.RefreshRate.Numerator = 0;//60;
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
				nullptr,									// ���� �ӽ�
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
				&m_pDeviceContext);
		};

		auto result = createDeviceAndSwapChain();
		if (result == DXGI_ERROR_SDK_COMPONENT_MISSING)
		{
			deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
			result = createDeviceAndSwapChain();
		}

		return true;
	}

	// ���޹��� ũ��� Default RenderTargetView, DepthStencilView�� �����Ѵ�.
	bool Graphics::UpdateSwapChain(uint32_t width, uint32_t height)
	{
		m_pDeviceContext->OMSetRenderTargets(0, NULL, NULL);
		DV_RELEASE(m_pDefaultRenderTargetView);
		DV_RELEASE(m_pDefaultDepthStencilTexture);
		DV_RELEASE(m_pDefaultDepthStencilView);

		for (uint32_t i = 0; i < MAX_RENDERTARGETS; ++i)
			m_pCurRenderTargetViews[i] = nullptr;
		m_pCurDepthStencilView = nullptr;

		m_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		// ����۸� ���� ����Ʈ ����Ÿ�ٺ� ����
		ID3D11Texture2D* pBackbufferTexture = nullptr;
		if (FAILED(m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - �ĸ� ���� �ؽ��� ������ �����Ͽ����ϴ�.");
			return false;
		}

		uint32_t curWidth = width;
		uint32_t curHeight = height;

		if (width == 0 || height == 0)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			m_pSwapChain->GetDesc(&swapChainDesc);

			curWidth = swapChainDesc.BufferDesc.Width;
			curHeight = swapChainDesc.BufferDesc.Height;
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), nullptr, &m_pDefaultRenderTargetView)))
		{
			DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - �ĸ� ���� ����Ÿ�ٺ� ������ �����Ͽ����ϴ�.");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		{
			// ����Ʈ �������ٽ� ����
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = curWidth;
			desc.Height = curHeight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.SampleDesc.Count = 1;// static_cast<UINT>(screenParams_.multiSample_);
			desc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(desc.Format, screenParams_.multiSample_);
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pDefaultDepthStencilTexture)))
			{
				DV_RELEASE(m_pDefaultDepthStencilTexture);
				DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - ����� ���� ���ٽ� �ؽ��� ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = 0;

			if (FAILED(m_pDevice->CreateDepthStencilView(
				static_cast<ID3D11Resource*>(m_pDefaultDepthStencilTexture),
				&desc,//nullptr,		// urho�� nullptr�� �����ߴ�. ���ó� sampler ������ �ذ���� �ʾҴ�.
				&m_pDefaultDepthStencilView)))
			{
				DV_RELEASE(m_pDefaultDepthStencilView);
				DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - ����� ���� ���ٽ� �� ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		m_Width = static_cast<int32_t>(curWidth);
		m_Height = static_cast<int32_t>(curHeight);

		ResetRenderTargets();

		m_bRenderTargetsDirty = true;

		return true;
	}

	bool Graphics::IsDeviceLost()
	{
		if (FAILED(m_pDevice->GetDeviceRemovedReason()))
			return true;

		return false;
	}

	bool Graphics::BeginFrame()
	{
		if (!IsInitialized())
			return false;

		// ��üȭ���̸鼭 �ּ�ȭ �Ǿ� �ִٸ� false?
		// => ������ ������ �ƴ� �� �ѵ�...? 

		ResetRenderTargets();

		for (uint32_t i = 0; i < 16; ++i)
			SetTexture(i, nullptr);

		FIRE_EVENT(BeginRenderEvent());

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		FIRE_EVENT(EndRenderEvent());

		// vsync ���θ� ����� ������ �� ��
		// orho�� screenParam���� ���� ������.
		m_pSwapChain->Present(1, 0);
	}

	// flags�� clear target flag��.
	// �� ���� �����ϰ� enum���� ������ �� �� �ϴ�.
	void Graphics::Clear(int32_t flags, const DirectX::XMFLOAT4& color, float depth, int32_t stencil)
	{
		prepareDraw();

		// render target view clear
		//if(flags & eClearTarget::Color)
		{
			float colors[]{ color.x, color.y, color.z, color.w };
			for (int32_t i = 0; i < 4; ++i)
			{
				if (m_pCurRenderTargetViews[i])
					m_pDeviceContext->ClearRenderTargetView(m_pCurRenderTargetViews[i], colors);
			}
		}

		// depth stencil view clear
		//if(flags & (eClearTarget::Depth | eClearTarget::Stencil))
		{
			m_pDeviceContext->ClearDepthStencilView(m_pCurDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		}
	}

	void Graphics::Draw(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t vertexCount, uint32_t vertexStart)
	{
		if (!vertexCount)
			return;

		prepareDraw();

		if (m_PrimitiveType != type)
		{
			m_pDeviceContext->IASetPrimitiveTopology(type);
			m_PrimitiveType = type;
		}

		m_pDeviceContext->Draw(vertexCount, vertexStart);
	}

	void Graphics::DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t indexCount, uint32_t indexStart, uint32_t vertexStart)
	{
		// �� ������ VertexCount�� ���� �� ���� Ȯ���ϴ� ������ ������ �ϴ� �ǳʶ��

		prepareDraw();

		if (m_PrimitiveType != type)
		{
			m_pDeviceContext->IASetPrimitiveTopology(type);
			m_PrimitiveType = type;
		}

		// urho�� ������ ����� ���̷� ���� �ΰ��� �Լ��� �����ߴ�.
		// ���� �� �������� ������ �������� ���ߴ�. 
		m_pDeviceContext->DrawIndexed(indexCount, indexStart, vertexStart);
	}

	void Graphics::DrawIndexedInstanced(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t indexCount, uint32_t instanceCount, uint32_t indexStart)
	{
	}

	void Graphics::SetDepthStencil(RenderTexture* pTexture)
	{
		// m_pDepthStencil�� Texture2D*�̴�.
		// ��, �߸� ���� ���̴�.
		if (m_pDepthStencil != pTexture)
			m_pDepthStencil = pTexture;

		m_bRenderTargetsDirty = true;
	}

	ID3D11RenderTargetView* Graphics::GetRenderTarget(uint32_t index) const
	{
		return index < MAX_RENDERTARGETS ? m_pRenderTargets[index]->GetRenderTargetView() : nullptr;
	}

	// Textur2D�� �ƴ϶� Texture�� �޾ƾ� ���� ������?
	// => urho�� RenderSurface��� ��ü�� �޴´�.
	// �ƴϸ� ����ó�� Texture2D�� �ް� �ű⿡�� RenderSurface�� ���Ϲ޾� �ٽ� ���� �Լ��� �����ϴ� ����̴�.
	// DepthStencil ���� ������ ������� �ٷ��.
	// => RenderSurface�� ����Ƽ�� RenderTarget���� ġȯ�ϸ� ���� ������?
	void Graphics::SetRenderTarget(uint32_t index, RenderTexture* pTexture)
	{
		if (index >= MAX_RENDERTARGETS)
			return;

		if (pTexture != m_pRenderTargets[index])
		{
			m_pRenderTargets[index] = pTexture;
			m_bRenderTargetsDirty = true;

			if (pTexture)
			{
				// ���� RenderTarget�� Mipmap�� �������� �ʴ´�.
				//if (pTexture->GetMipmapCount() > 1)
				//	pTexture->SetMipLevelsDirty();
			}
		}
	}

	void Graphics::ResetRenderTargets()
	{
		// render targets
		for (uint32_t i = 0; i < MAX_RENDERTARGETS; ++i)
			SetRenderTarget(i, nullptr);

		// depth stencil
		SetDepthStencil(nullptr);

		// viewport
		RECT viewportRect;
		viewportRect.left = 0;
		viewportRect.right = static_cast<LONG>(m_Width);
		viewportRect.top = 0;
		viewportRect.bottom = static_cast<LONG>(m_Height);

		SetViewportRect(viewportRect);
	}

	RECT Graphics::GetViewportRect() const
	{
		RECT rect;
		rect.left = static_cast<LONG>(m_Viewport.TopLeftX);
		rect.right = rect.left + static_cast<LONG>(m_Viewport.Width);
		rect.top = static_cast<LONG>(m_Viewport.TopLeftY);
		rect.bottom = rect.top + static_cast<LONG>(m_Viewport.Height);

		return rect;
	}

	void Graphics::SetViewport(const D3D11_VIEWPORT& viewport)
	{
		m_Viewport = viewport;

		m_pDeviceContext->RSSetViewports(1, &m_Viewport);
	}

	void Graphics::SetViewportRect(const RECT& rect)
	{
		D3D11_VIEWPORT viewport;

		viewport.TopLeftX = static_cast<float>(rect.left);
		viewport.TopLeftY = static_cast<float>(rect.top);

		viewport.Width = static_cast<float>(rect.right - rect.left);
		viewport.Height = static_cast<float>(rect.bottom - rect.top);

		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		SetViewport(viewport);
	}

	VertexBuffer* Graphics::GetVertexBuffer(uint32_t index) const
	{
		//return index < MAX_VERTEX_STREAMS ? m_pVertexBuffers[index] : nullptr;
		return m_pVertexBuffer;
	}

	void Graphics::SetVertexBuffer(VertexBuffer* pBuffer)
	{
		if (m_pVertexBuffer != pBuffer)
		{
			m_pVertexBuffer = pBuffer;
			m_bVertexTypeDirty = true;
		}
	}

	void Graphics::SetIndexBuffer(IndexBuffer* pBuffer)
	{
		if (m_pIndexBuffer != pBuffer)
		{
			if (pBuffer)
			{
				m_pDeviceContext->IASetIndexBuffer(
					pBuffer->GetBuffer(),
					pBuffer->GetFormat(),
					0);
			}
			else
				m_pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

			m_pIndexBuffer = pBuffer;
		}
	}

	ShaderVariation* Graphics::GetShader(eShaderType type, const std::string& name, const std::string& defines)
	{
		// �̸� �� �� lastShader ���� ���θ� ���� ���� ���θ� �Ǵ��Ѵ�.
		if (m_LastShaderName != name || !m_pLastShader)
		{
			auto pCache = GetSubsystem<ResourceCache>();

			// ���� name�� �׳� �̸��� ���޹޴´�.
			// ���⿡�� shader�� ��ο� hlsl�� �ٿ��־�� �Ѵ�.

			// ������ Cache�� Exists��� �Լ��� �־�� �Ѵ�.
			if (m_LastShaderName == name && !pCache->GetExistingResource<Shader>(name))
				return nullptr;

			m_pLastShader = pCache->GetResource<Shader>(name);
			m_LastShaderName = name;
		}

		// ���������� ���� �������� ���� ���� ���´�.
		return m_pLastShader ? m_pLastShader->GetVariation(type, defines) : nullptr;
	}

	// �ϴ� ���̴��� ���� ���̴��� �ٸ��ٸ� ���ε��Ѵ�.
	// ���� �ش� ���̴��� ����ϴ� ������۸� ���� ������ۿ� ���� �� 
	// ��������� �ִٸ� ���ε��Ѵ�.
	void Graphics::SetShaders(ShaderVariation* pVertexShader, ShaderVariation* pPixelShader)
	{
		if (m_pVertexShaderVariation == pVertexShader && m_pPixelShaderVariation == pPixelShader)
			return;

		if (m_pVertexShaderVariation != pVertexShader)
		{
			// �� �κ��� ������ ���� �� ����.
			// �и��� NULL�� ���޹��� ���� �ִ�.
			if (!pVertexShader->GetShader())
				pVertexShader->CompileAndCreate();

			m_pDeviceContext->VSSetShader(
				static_cast<ID3D11VertexShader*>(pVertexShader ? pVertexShader->GetShader() : nullptr),
				nullptr, 0);

			m_pVertexShaderVariation = pVertexShader;
			m_bVertexTypeDirty = true;
		}

		if (m_pPixelShaderVariation != pPixelShader)
		{
			if (!pPixelShader->GetShader())
				pPixelShader->CompileAndCreate();

			m_pDeviceContext->PSSetShader(
				static_cast<ID3D11PixelShader*>(pPixelShader ? pPixelShader->GetShader() : nullptr),
				nullptr, 0);

			m_pPixelShaderVariation = pPixelShader;
		}

		// �ϴ� ���� ���̴��� ���۸� �����´�.
		// �� ���̴��� �� �� �־�� �ϴ� ���ϱ�?
		if (m_pVertexShaderVariation && m_pPixelShaderVariation)
		{
			auto key = std::pair(m_pVertexShaderVariation, m_pPixelShaderVariation);
			auto it = m_ShaderPrograms.find(key);
			if (it != m_ShaderPrograms.end())
				m_pCurShaderProgram = it->second;
			else
			{
				// ���� ���⿡���� ����Ѵٸ� unique�� �´�.
				ShaderProgram* pNewShaderProgram = new ShaderProgram(this, m_pVertexShaderVariation, m_pPixelShaderVariation);
				m_ShaderPrograms[key] = pNewShaderProgram;
				m_pCurShaderProgram = pNewShaderProgram;
			}
		}

		bool bVSCBufferChanged = false;
		bool bPSCBufferChanged = false;

		for (uint32_t i = 0; i < 7; ++i)
		{
			auto pVSConstantBuffer = m_pCurShaderProgram->m_pVSConstantBuffers[i] ?
				m_pCurShaderProgram->m_pVSConstantBuffers[i]->GetBuffer() : nullptr;

			if (pVSConstantBuffer != m_pCurVertexShaderConstantBuffers[i])
			{
				m_pCurVertexShaderConstantBuffers[i] = pVSConstantBuffer;
				bVSCBufferChanged = true;
			}

			auto pPSConstantBuffer = m_pCurShaderProgram->m_pPSConstantBuffers[i] ?
				m_pCurShaderProgram->m_pPSConstantBuffers[i]->GetBuffer() : nullptr;

			if (pPSConstantBuffer != m_pCurPixelShaderConstantBuffers[i])
			{
				m_pCurPixelShaderConstantBuffers[i] = pPSConstantBuffer;
				bPSCBufferChanged = true;
			}
		}

		if (bVSCBufferChanged)
			m_pDeviceContext->VSSetConstantBuffers(0, 7, &m_pCurVertexShaderConstantBuffers[0]);
		if (bPSCBufferChanged)
			m_pDeviceContext->PSSetConstantBuffers(0, 7, &m_pCurPixelShaderConstantBuffers[0]);
	}

	void Graphics::SetShaderParameter(const std::string& param, bool value)
	{
		if (!m_pCurShaderProgram)
			return;

		auto it = m_pCurShaderProgram->m_Parameters.find(param);
		if (it == m_pCurShaderProgram->m_Parameters.end())
			return;

		ConstantBuffer* pBuffer = it->second.pBuffer;
		if (pBuffer)
		{
			if (!pBuffer->IsDirty())
				m_DirtyConstantBuffers.emplace_back(pBuffer);

			pBuffer->SetParameter(it->second.Offset, sizeof(bool), &value);
		}
	}

	void Graphics::SetShaderParameter(const std::string& param, float value)
	{
		if (!m_pCurShaderProgram)
			return;

		auto it = m_pCurShaderProgram->m_Parameters.find(param);
		if (it == m_pCurShaderProgram->m_Parameters.end())
			return;

		ConstantBuffer* pBuffer = it->second.pBuffer;
		if (pBuffer)
		{
			if (!pBuffer->IsDirty())
				m_DirtyConstantBuffers.emplace_back(pBuffer);

			pBuffer->SetParameter(it->second.Offset, sizeof(float), &value);
		}
	}

	void Graphics::SetShaderParameter(const std::string& param, int32_t value)
	{
		if (!m_pCurShaderProgram)
			return;

		auto it = m_pCurShaderProgram->m_Parameters.find(param);
		if (it == m_pCurShaderProgram->m_Parameters.end())
			return;

		ConstantBuffer* pBuffer = it->second.pBuffer;
		if (pBuffer)
		{
			if (!pBuffer->IsDirty())
				m_DirtyConstantBuffers.emplace_back(pBuffer);

			pBuffer->SetParameter(it->second.Offset, sizeof(int32_t), &value);
		}
	}

	void Graphics::SetShaderParameter(const std::string& param, const DirectX::XMFLOAT2& vector)
	{
		if (!m_pCurShaderProgram)
			return;

		auto it = m_pCurShaderProgram->m_Parameters.find(param);
		if (it == m_pCurShaderProgram->m_Parameters.end())
			return;

		ConstantBuffer* pBuffer = it->second.pBuffer;
		if (pBuffer)
		{
			if (!pBuffer->IsDirty())
				m_DirtyConstantBuffers.emplace_back(pBuffer);

			pBuffer->SetParameter(it->second.Offset, sizeof(DirectX::XMFLOAT2), &vector);
		}
	}

	void Graphics::SetShaderParameter(const std::string& param, const DirectX::XMFLOAT3& vector)
	{
		if (!m_pCurShaderProgram)
			return;

		auto it = m_pCurShaderProgram->m_Parameters.find(param);
		if (it == m_pCurShaderProgram->m_Parameters.end())
			return;

		ConstantBuffer* pBuffer = it->second.pBuffer;
		if (pBuffer)
		{
			if (!pBuffer->IsDirty())
				m_DirtyConstantBuffers.emplace_back(pBuffer);

			pBuffer->SetParameter(it->second.Offset, sizeof(DirectX::XMFLOAT3), &vector);
		}
	}

	void Graphics::SetShaderParameter(const std::string& param, const DirectX::XMFLOAT4& vector)
	{
		if (!m_pCurShaderProgram)
			return;

		auto it = m_pCurShaderProgram->m_Parameters.find(param);
		if (it == m_pCurShaderProgram->m_Parameters.end())
			return;

		ConstantBuffer* pBuffer = it->second.pBuffer;
		if (pBuffer)
		{
			if (!pBuffer->IsDirty())
				m_DirtyConstantBuffers.emplace_back(pBuffer);

			pBuffer->SetParameter(it->second.Offset, sizeof(DirectX::XMFLOAT4), &vector);
		}
	}

	void Graphics::SetShaderParameter(const std::string& param, const DirectX::XMFLOAT4X4& matrix)
	{
		if (!m_pCurShaderProgram)
			return;

		auto it = m_pCurShaderProgram->m_Parameters.find(param);
		if (it == m_pCurShaderProgram->m_Parameters.end())
			return;

		ConstantBuffer* pBuffer = it->second.pBuffer;
		if (pBuffer)
		{
			if (!pBuffer->IsDirty())
				m_DirtyConstantBuffers.emplace_back(pBuffer);

			pBuffer->SetParameter(it->second.Offset, sizeof(DirectX::XMFLOAT4X4), &matrix);
		}
	}

	void Graphics::SetShaderParameter(const std::string& param, const DirectX::XMMATRIX& matrix)
	{
		if (!m_pCurShaderProgram)
			return;

		auto it = m_pCurShaderProgram->m_Parameters.find(param);
		if (it == m_pCurShaderProgram->m_Parameters.end())
			return;

		ConstantBuffer* pBuffer = it->second.pBuffer;
		if (pBuffer)
		{
			if (!pBuffer->IsDirty())
				m_DirtyConstantBuffers.emplace_back(pBuffer);

			pBuffer->SetParameter(it->second.Offset, sizeof(DirectX::XMMATRIX), &matrix);
		}
	}

	bool Graphics::HasTextureUnit(eTextureUnit unit)
	{
		return (m_pVertexShaderVariation && m_pVertexShaderVariation->HasTextureUnit(unit)) ||
			(m_pPixelShaderVariation && m_pPixelShaderVariation->HasTextureUnit(unit));
	}

	ConstantBuffer* Graphics::GetOrCreateConstantBuffer(eShaderType type, uint32_t index, uint32_t size)
	{
		uint32_t key = static_cast<uint32_t>(type) | (index << 1) | (size << 4);
		auto it = m_AllConstantBuffers.find(key);
		if (it != m_AllConstantBuffers.end())
			return it->second;
		else
		{
			auto pNewConstantBuffer = new ConstantBuffer(m_pContext);
			pNewConstantBuffer->SetSize(size);
			m_AllConstantBuffers[key] = pNewConstantBuffer;
			return pNewConstantBuffer;
		}
	}

	Texture* Graphics::GetTexture(uint32_t index)
	{
		return index < static_cast<uint32_t>(eTextureUnit::Max) ? m_pTextures[index] : nullptr;
	}

	// urho�� ��� Texture�� �ް� GetShaderResourceView(), GetSampler() ���� ���� ȣ���Ͽ� �����Ѵ�.
	// ��, ������ ������ �����ϴ�.
	// �ݸ� RenderTarget�� ��� RenderSurface��� ��ü�� �����Ѵ�.
	void Graphics::SetTexture(uint32_t index, Texture* pTexture)
	{
		if (index >= static_cast<uint32_t>(eTextureUnit::Max))
			return;

		if (pTexture)
		{
			// ����Ÿ�� 0�� ������ �ּҶ��
			// ����� ����Ѵ�?

			if (pTexture->IsMipLevelsDirty())
				pTexture->UpdateMipLevels();

			if (pTexture->IsSamplerStateDirty())
			{
				pTexture->UpdateSamplerState();
				m_pTextures[index] = nullptr;
			}
		}

		if (pTexture != m_pTextures[index])
		{
			if (m_FirstDirtyTexture == 0xffffffff)
				m_FirstDirtyTexture = m_LastDirtyTexture = index;
			else
			{
				if (index < m_FirstDirtyTexture)
					m_FirstDirtyTexture = index;
				if (index > m_LastDirtyTexture)
					m_LastDirtyTexture = index;
			}

			m_pTextures[index] = pTexture;
			m_pShaderResourceViews[index] = pTexture ? pTexture->GetShaderResourceView() : nullptr;
			m_pSamplers[index] = pTexture ? pTexture->GetSamplerState() : nullptr;
			m_bTextureDirty = true;
		}
	}

	void Graphics::OnResizeWindow()
	{
		if (!IsInitialized())
			return;

		UpdateSwapChain();

		// render targets�� + viewport �� ���� ������ �Ѵ�.
		// �׷��� orho�� reset�� �Ѵ�. �Ƹ� �ٸ� ������ ���� ���峪 ����.

		// �� �Լ��� �Բ� ������ ��� ������ �Ͼ�� �ٸ� �Լ�(OnScreenModeChanged)�� ScreenMode �̺�Ʈ�� �߻���Ų��.
		// �̶� �Ű������� ũ��, ���(Ǯ ��ũ��, ũ�� ���� ����), ����� ���� ������ �����Ѵ�.
	}

	// �̸��� �ٲ����� ���ڴ�.
	// ���� bind�� �����ϴ� �����̴�.
	void Graphics::prepareDraw()
	{
		if (m_bRenderTargetsDirty)
		{
			// �̷����ϸ� ID3D11DepthstencilView�� ������ ����ϰ� �ȴ�.
			// �����δ� nullptr�� ��Ȳ�� �ִ�.
			m_pCurDepthStencilView = (m_pDepthStencil && m_pDepthStencil->GetUsage() == eTextureUsage::DepthStencil) ?
				m_pDepthStencil->GetDepthStencilView() : m_pDefaultDepthStencilView;
			// write�� �ƴ϶�� read only

			for (uint32_t i = 0; i < MAX_RENDERTARGETS; ++i)
			{
				m_pCurRenderTargetViews[i] = (m_pRenderTargets[i] && m_pRenderTargets[i]->GetUsage() == eTextureUsage::RenderTarget) ?
					m_pRenderTargets[i]->GetRenderTargetView() : nullptr;
			}

			if (!m_pRenderTargets[0] && (!m_pDepthStencil ||
				(m_pDepthStencil && m_pDepthStencil->GetWidth() == m_Width && m_pDepthStencil->GetHeight() == m_Height)))
			{
				m_pCurRenderTargetViews[0] = m_pDefaultRenderTargetView;
			}

			m_pDeviceContext->OMSetRenderTargets(MAX_RENDERTARGETS, &m_pCurRenderTargetViews[0], m_pCurDepthStencilView);//nullptr);

			m_bRenderTargetsDirty = false;
		}

		// shader resources + samplers
		if (m_bTextureDirty && m_FirstDirtyTexture < 0xffffffff)
		{
			m_pDeviceContext->PSSetShaderResources(m_FirstDirtyTexture, m_LastDirtyTexture - m_FirstDirtyTexture + 1,
				&m_pShaderResourceViews[m_FirstDirtyTexture]);
			m_pDeviceContext->PSSetSamplers(m_FirstDirtyTexture, m_LastDirtyTexture - m_FirstDirtyTexture + 1,
				&m_pSamplers[m_FirstDirtyTexture]);

			m_FirstDirtyTexture = m_LastDirtyTexture = 0xffffffff;
			m_bTextureDirty = false;
		}

		// vertex buffer + inputlayout
		if (m_bVertexTypeDirty && m_pVertexShaderVariation && m_pVertexShaderVariation->GetShader())
		{
			// vertex buffer ���� �� ������ set�Ѵ�.
			uint32_t stride = m_pVertexBuffer->GetSize();
			uint32_t offset = 0;
			auto pVb = m_pVertexBuffer->GetBuffer();

			// ���۸� �迭�� ������ ��� ����������.
			m_pDeviceContext->IASetVertexBuffers(0, 1, &pVb, &stride, &offset);

			if (m_bVertexTypeDirty)
			{
				if (m_pVertexBuffer)
				{
					// VertexBuffer�� ElementsHash�� VertexShaderVariation�� SemanticsHash�� ������ �ʿ䰡 ����.
					// �ٸ� ElementsHash�� �����ϴ� Semantice�� �������� Ȯ���� �ʿ��ϴ�.
					// ������ urho������ �̸� Ȯ���ϴ� �������� ����.
					// �ٸ� Buffer�� Shader�� �������� ���ο� Hash�� ���� ���̴�.
					uint64_t key = 0;
					key = m_pVertexBuffer->GetElementsHash();
					if (key)
					{
						// ���⿡ Shader�� Hash�� ���� Hash�� �����Ѵ�.

						// �׸��� ���� �ؽ��� ���Ͽ� �Ʒ����� ����� ���θ� �Ǵ��Ѵ�.
						if (m_InputLayoutHash != key)
						{
							auto it = m_InputLayouts.find(key);
							if (it != m_InputLayouts.end())
							{
								m_pDeviceContext->IASetInputLayout(it->second->GetInputLayout());
							}
							else
							{
								auto pNewInputLayout = new InputLayout(m_pContext, m_pVertexBuffer, m_pVertexShaderVariation);
								m_InputLayouts.emplace(key, pNewInputLayout);
								m_pDeviceContext->IASetInputLayout(pNewInputLayout->GetInputLayout());
							}

							m_InputLayoutHash = key;
						}
					}
				}
			}

			m_bVertexTypeDirty = false;
		}

		// blend state

		// depthstencil state
		{
			// �ӽ� ����
			if (!m_pDepthStencilState)
			{
				D3D11_DEPTH_STENCIL_DESC desc;
				ZeroMemory(&desc, sizeof(desc));

				// Set up the description of the stencil state.
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

				if (FAILED(m_pDevice->CreateDepthStencilState(&desc, &m_pDepthStencilState)))
				{
					DV_LOG_ENGINE_ERROR("DepthStencilState ������ �����Ͽ����ϴ�.");
					return;
				}
			}
			m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
		}

		// rasterizer state
		{
			// �ӽ� ����
			if (!m_pRasterizerState)
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

				if (FAILED(m_pDevice->CreateRasterizerState(&desc, &m_pRasterizerState)))
				{
					DV_LOG_ENGINE_ERROR("RasterizerState ������ �����Ͽ����ϴ�.");
					return;
				}
			}
			m_pDeviceContext->RSSetState(m_pRasterizerState);
		}

		// RSSetScissorRect

		for (auto pConstantBuffer : m_DirtyConstantBuffers)
			pConstantBuffer->Update();
		m_DirtyConstantBuffers.clear();
	}

	void RegisterGraphicsObject(Context* pContext)
	{
		Texture2D::RegisterObject(pContext);
		
		Model::RegisterObject(pContext);
		Shader::RegisterObject(pContext);
		Material::RegisterObject(pContext);

		Texture2D::RegisterObject(pContext);
	}
}