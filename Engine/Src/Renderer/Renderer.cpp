#include "divepch.h"
#include "Renderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Base/Base.h"
#include "Base/Engine.h"

namespace Dive
{
	// static varialbes
	GraphicsDevice Renderer::m_GraphicsDevice;

	unsigned int Renderer::m_TextureWidth = 0;
	unsigned int Renderer::m_TextureHeight = 0;

	Texture2D* Renderer::m_pSampleTex = nullptr;
	Texture2D* Renderer::m_pDepthStencilTex = nullptr;

	ID3D11DepthStencilState* Renderer::m_pDepthStencilState = nullptr;

	void Renderer::Initialize(const WindowData* pData)
	{
		DV_ASSERT(pData && "WnidowData가 존재하지 않습니다.");

		m_GraphicsDevice.Initialize(pData);

		// 꼭 필요한 리소스들이다.
		// 따라서 어느 하나라도 생성에 실패하면 종료하는 것이 맞다.
		// 이를 위해선 bool로 확인이 필요하며
		// 어느 부분에서 잘못되었는지 개별 추적이 가능해야 한다.
		SetTextures(pData->Width, pData->Height);
		createSamplers();
		createDepthStencilStates();
		createRasterizerStates();
		createShaders();
	}

	void Renderer::Shutdown()
	{
		// 함수로 만들어야 하나...?
		DV_DELETE(m_pDepthStencilTex);
		DV_DELETE(m_pSampleTex);

		m_GraphicsDevice.Shutdown();
	}
	
	// Camera를 받아 Data(viewport)를 저장
	// clear, set은 다른 곳에서?
	// Hazle은 오버로딩 되어있다.
	void Renderer::BeginScene()
	{
		auto pImmediateContext = m_GraphicsDevice.GetImmediateContext();
		auto pRenderTargetView = m_pSampleTex ? m_pSampleTex->GetRenderTargetView() : nullptr;
		auto pDepthStencilView = m_pDepthStencilTex ? m_pDepthStencilTex->GetDepthStencilView() : nullptr;
		if (!pImmediateContext || !pRenderTargetView)
			return;

		// active camera로부터 가져와야 한다.
		float clearColors[4] = {0.5f, 0.5f, 1.0f, 1.0f};

		pImmediateContext->ClearRenderTargetView(pRenderTargetView, clearColors);
		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	}
	
	void Renderer::EndScene()
	{
		auto pSwapChain = m_GraphicsDevice.GetSwapChain();
		if (!pSwapChain)
			return;

		pSwapChain->Present(m_GraphicsDevice.IsVSync() ? 1 : 0, 0);
	}

	void Renderer::SetResolution(unsigned int width, unsigned int height)
	{
		m_GraphicsDevice.ResizeBackBuffer(width, height);
	}

	// 이 곳에선 크기 확인 및 저장 후 createRenderTargets()를 호출하는 편이 나을 것 같다.
	void Renderer::SetTextures(unsigned int width, unsigned int height)
	{
		if (m_TextureWidth == width && m_TextureHeight == height)
			return;

		m_TextureWidth = width;
		m_TextureHeight = height;

		createRenderTargets();
	}

	void Renderer::createRenderTargets()
	{
		unsigned int width = m_TextureWidth;
		unsigned int height = m_TextureHeight;
		
		// Render Target Textures
		DV_DELETE(m_pSampleTex);
		m_pSampleTex = Texture2D::Create(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, true);

		// Depth Stencil Buffers
		DV_DELETE(m_pDepthStencilTex);
		m_pDepthStencilTex = Texture2D::Create(width, height, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT);
	}

	void Renderer::createSamplers()
	{
		// 이런 애들의 생성 확인이 문제다.
		// 구조체 혹은 클래스화하기엔 너무 부피가 작고, 이름을 일일이 붙이기도 애매하다.
		// 그렇다면 결국 이 곳, 혹은 Graphics의 생성함수에서 생성 오류 로그를 출력해야 한다.
		// 그리고 이는 통일성이 있어야 한다.
	}

	void Renderer::createDepthStencilStates()
	{
		
	}
	
	void Renderer::createRasterizerStates()
	{
		
	}
	
	// 개별로 체크하는게 맞는 것 같다.
	// 따라서 이 함수가 void일 필요는 없다.
	void Renderer::createShaders()
	{
		ID3D11PixelShader* pPixelShader = nullptr;
		if (!m_GraphicsDevice.CreatePixelShader("Assets/Shaders/none.hlsl", &pPixelShader))
		{
			DV_CORE_WARN("ㅇㅇ 없어.");
			
		}
	}
}