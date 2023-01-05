#include "divepch.h"
#include "Graphics.h"
#include "GraphicsEvents.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "InputLayout.h"
#include "Texture2D.h"
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
		if (FAILED(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&m_pDevice,
			nullptr,
			&m_pDeviceContext)))
		{
			DV_LOG_ENGINE_ERROR("Graphics::createDeviceAndSwapChain - D3D11 장치 생성에 실패하였습니다.");
			return false;
		}

		// 멀티 샘플 레밸 체크?

		IDXGIDevice* pDxgiDevice = nullptr;
		m_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter = nullptr;
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory = nullptr;
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		// 동일 해상도에서 최적화된 리플래쉬 레이트 설정
		// 일단 건너뛴다.

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 1;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB???
		desc.BufferDesc.RefreshRate.Denominator = 1;			// 추후 수정(vsync에 따라 달라진다?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// 멀티 샘플링 off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = IsFullScreen() ? FALSE : TRUE;	// orho는 TRUE 고정이다. 전체화면은 여기에서 결정된다.
		desc.OutputWindow = m_WindowHandle;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek에선 0이고 다른 값들 설정이 남아 있다...

		if (FAILED(pDxgiFactory->CreateSwapChain(m_pDevice, &desc, &m_pSwapChain)))
		{
			DV_LOG_ENGINE_ERROR("Graphics::createDeviceAndSwapChain - D3D11 스왑체인 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	// 전달받은 크기로 Default RenderTargetView, DepthStencilView를 생성한다.
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

		// 백버퍼를 얻어와 디폴트 렌더타겟뷰 생성
		ID3D11Texture2D* pBackbufferTexture = nullptr;
		if (FAILED(m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - 후면 버퍼 텍스쳐 생성에 실패하였습니다.");
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
			DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - 후면 버퍼 렌더타겟뷰 생성에 실패하였습니다.");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		// 디폴트 뎁스스텐실 생성
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
			DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - 백버퍼 깊이 스텐실 텍스쳐 생성에 실패하였습니다.");
			return false;
		}

		if (FAILED(m_pDevice->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(m_pDefaultDepthStencilTexture),
			nullptr,
			&m_pDefaultDepthStencilView)))
		{
			DV_LOG_ENGINE_ERROR("Graphics::updateSwapChain - 백버퍼 깊이 스텐실 뷰 생성에 실패하였습니다.");
			return false;
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

		// 전체화면이면서 최소화 되어 있다면 false?
		// => 가능한 조합이 아닌 듯 한데...? 

		ResetRenderTargets();

		// SetTexture
		// 16개를 전부 nullptr로 한다.
		// 어디에 사용하는 텍스쳐인지는 모르겠다.
		// => shader resource view 같다. 즉, 모델에 붙일 텍스쳐 말이다.

		FIRE_EVENT(BeginRenderEvent());

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		FIRE_EVENT(EndRenderEvent());

		// vsync 여부를 멤버로 가져야 할 듯
		// orho는 screenParam으로 전부 가진다.
		m_pSwapChain->Present(1, 0);
	}

	// flags는 clear target flag다.
	// 논리 합이 가능하게 enum으로 만들어야 할 듯 하다.
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
		// 이 곳에도 VertexCount를 받은 후 개수 확인하는 구문이 있으나 일단 건너띄기

		prepareDraw();

		if (m_PrimitiveType != type)
		{
			m_pDeviceContext->IASetPrimitiveTopology(type);
			m_PrimitiveType = type;
		}

		// urho는 마지막 요소의 차이로 인해 두개의 함수로 구현했다.
		// 아직 그 차이점을 완전히 이해하지 못했다. 
		m_pDeviceContext->DrawIndexed(indexCount, indexStart, vertexStart);
	}

	void Graphics::DrawIndexedInstanced(D3D11_PRIMITIVE_TOPOLOGY type, uint32_t indexCount, uint32_t instanceCount, uint32_t indexStart)
	{
	}

	void Graphics::SetDepthStencil(Texture2D* pTexture)
	{
		if (m_pDepthStencil != pTexture)
			m_pDepthStencil = pTexture;

		m_bRenderTargetsDirty = true;

		// 이 곳에서 raseterizer dirty를 true로 한다. 그런데 아래 함수에선 안한다....
	}

	ID3D11RenderTargetView* Graphics::GetRenderTarget(uint32_t index) const
	{
		return index < MAX_RENDERTARGETS ? m_pRenderTargets[index]->GetRenderTargetView() : nullptr;
	}

	// Textur2D가 아니라 Texture를 받아야 하지 않을까?
	void Graphics::SetRenderTarget(uint32_t index, Texture2D* pTexture)
	{
		if (index >= MAX_RENDERTARGETS)
			return;

		if (pTexture != m_pRenderTargets[index])
		{
			m_pRenderTargets[index] = pTexture;
			m_bRenderTargetsDirty = true;

			if (pTexture)
			{
				if (pTexture->GetMipmapCount() > 1)
					pTexture->SetMipLevelsDirty();

				// 이외에도 몇가지 처리를 더 하는데... 잘 모르겠다.
				// 그런데 SetTexture를 호출해야만 밉맵을 생성할텐데...
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
		// 이름 비교 및 lastShader 존재 여부를 통해 진입 여부를 판단한다.
		if (m_LastShaderName != name || !m_pLastShader)
		{
			auto pCache = GetSubsystem<ResourceCache>();

			// 원래 name은 그냥 이름만 전달받는다.
			// 여기에서 shader의 경로와 hlsl을 붙여주어야 한다.

			// 원래는 Cache에 Exists라는 함수가 있어야 한다.
			if (m_LastShaderName == name && !pCache->GetExistingResource<Shader>(name))
				return nullptr;

			m_pLastShader = pCache->GetResource<Shader>(name);
			m_LastShaderName = name;
		}

		// 생성까지만 했지 컴파일은 하지 않은 상태다.
		return m_pLastShader ? m_pLastShader->GetVariation(type, defines) : nullptr;
	}

	// 일단 셰이더가 이전 셰이더와 다르다면 바인딩한다.
	// 이후 해당 셰이더가 사용하는 상수버퍼를 이전 상수버퍼와 비교한 후 
	// 변경사항이 있다면 바인딩한다.
	void Graphics::SetShaders(ShaderVariation* pVertexShader, ShaderVariation* pPixelShader)
	{
		if (m_pVertexShaderVariation == pVertexShader && m_pPixelShaderVariation == pPixelShader)
			return;

		if (m_pVertexShaderVariation != pVertexShader)
		{
			// 이 부분은 문제가 생길 것 같다.
			// 분명히 NULL을 전달받을 수도 있다.
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

		// 일단 현재 셰이더의 버퍼를 가져온다.
		// 꼭 셰이더가 둘 다 있어야 하는 것일까?
		if (m_pVertexShaderVariation && m_pPixelShaderVariation)
		{
			auto key = std::pair(m_pVertexShaderVariation, m_pPixelShaderVariation);
			auto it = m_ShaderPrograms.find(key);
			if (it != m_ShaderPrograms.end())
				m_pCurShaderProgram = it->second;
			else
			{
				// 오직 여기에서만 사용한다면 unique가 맞다.
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

	void Graphics::SetTexture(uint32_t index, Texture* pTexture)
	{
		if (index >= static_cast<uint32_t>(eTextureUnit::Max))
			return;

		if (pTexture)
		{
			// 렌더타겟 0과 동일한 주소라면
			// 백업을 사용한다?

			if (pTexture->GetMipLevelsDirty())
				pTexture->SetMipLevelsDirty();

			if (pTexture->GetParametersDirty())
			{
				pTexture->UpdateParameters();
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
			m_bTextureDirty = true;
		}
	}

	// 다른 설정이 물려있다.
	bool Graphics::LoadShaders()
	{
		// 일단 생성 후 바로 등록이지만 추후 바뀔 수 있다.
		// depthstencilstate
		{

		}

		// rssetstate
		{
			ID3D11RasterizerState* pRasterizerState = nullptr;

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

			// Create the rasterizer state from the description we just filled out.
			auto result = m_pDevice->CreateRasterizerState(&desc, &pRasterizerState);
			if (FAILED(result))
			{
				return false;
			}

			// Now set the rasterizer state.
			m_pDeviceContext->RSSetState(pRasterizerState);
		}

		return true;
	}

	void Graphics::OnResizeWindow()
	{
		if (!IsInitialized())
			return;

		UpdateSwapChain();

		// render targets들 + viewport 도 새로 만들어야 한다.
		// 그런데 orho는 reset만 한다. 아마 다른 곳에서 새로 만드나 보다.

		// 이 함수와 함께 윈도우 모드 변경이 일어나는 다른 함수(OnScreenModeChanged)는 ScreenMode 이벤트를 발생시킨다.
		// 이때 매개변수로 크기, 모드(풀 스크린, 크기 변경 가능), 모니터 등의 정보를 전달한다.
	}

	// 이름을 바꿨으면 좋겠다.
	// 각종 bind를 수행하는 구문이다.
	void Graphics::prepareDraw()
	{
		if (m_bRenderTargetsDirty)
		{
			// 이렇게하면 ID3D11DepthstencilView를 무조건 사용하게 된다.
			m_pCurDepthStencilView = (m_pDepthStencil && m_pDepthStencil->GetUsage() == eTextureUsage::DepthStencil) ?
				m_pDepthStencil->GetDepthStencilView() : m_pDefaultDepthStencilView;
			// write가 아니라면 read only

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

			m_pDeviceContext->OMSetRenderTargets(MAX_RENDERTARGETS, &m_pCurRenderTargetViews[0], nullptr);// m_pCurDepthStencilView);

			m_bRenderTargetsDirty = false;
		}

		// shader resources + samplers
		if (m_bTextureDirty && m_FirstDirtyTexture < 0xffffffff)
		{
			auto pSrv = m_pTextures[m_FirstDirtyTexture] ? m_pTextures[m_FirstDirtyTexture]->GetShaderResourceView() : nullptr;

			// vertex shader
			m_pDeviceContext->VSSetShaderResources(m_FirstDirtyTexture, m_LastDirtyTexture - m_FirstDirtyTexture + 1, &pSrv);

			// pixel shader
			m_pDeviceContext->PSSetShaderResources(m_FirstDirtyTexture, m_LastDirtyTexture - m_FirstDirtyTexture + 1, &pSrv);

			m_FirstDirtyTexture = m_LastDirtyTexture = 0xffffffff;
			m_bTextureDirty = false;
		}

		// vertex buffer + inputlayout
		if (m_bVertexTypeDirty && m_pVertexShaderVariation && m_pVertexShaderVariation->GetShader())
		{
			// vertex buffer 역시 이 곳에서 set한다.
			uint32_t stride = m_pVertexBuffer->GetSize();
			uint32_t offset = 0;
			auto pVb = m_pVertexBuffer->GetBuffer();

			// 버퍼를 배열로 관리할 경우 복잡해진다.
			m_pDeviceContext->IASetVertexBuffers(0, 1, &pVb, &stride, &offset);

			if (m_bVertexTypeDirty)
			{
				if (m_pVertexBuffer)
				{
					// VertexBuffer의 ElementsHash와 VertexShaderVariation의 SemanticsHash가 동일할 필요가 없다.
					// 다만 ElementsHash에 존재하는 Semantice만 가지는지 확인이 필요하다.
					// 하지만 urho에서도 이를 확인하는 구문따윈 없다.
					// 다만 Buffer와 Shader의 조합으로 새로운 Hash를 만들 뿐이다.
					uint64_t key = 0;
					key = m_pVertexBuffer->GetElementsHash();
					if (key)
					{
						// 여기에 Shader의 Hash도 더해 Hash를 갱신한다.

						// 그리고 이전 해쉬와 비교하여 아래구문 재실행 여부를 판단한다.
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

		// rasterizer state

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
	}
}