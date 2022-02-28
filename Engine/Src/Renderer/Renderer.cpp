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
		DV_ASSERT(pData && "WnidowData�� �������� �ʽ��ϴ�.");

		m_GraphicsDevice.Initialize(pData);

		// �� �ʿ��� ���ҽ����̴�.
		// ���� ��� �ϳ��� ������ �����ϸ� �����ϴ� ���� �´�.
		// �̸� ���ؼ� bool�� Ȯ���� �ʿ��ϸ�
		// ��� �κп��� �߸��Ǿ����� ���� ������ �����ؾ� �Ѵ�.
		SetTextures(pData->Width, pData->Height);
		createSamplers();
		createDepthStencilStates();
		createRasterizerStates();
		createShaders();
	}

	void Renderer::Shutdown()
	{
		// �Լ��� ������ �ϳ�...?
		DV_DELETE(m_pDepthStencilTex);
		DV_DELETE(m_pSampleTex);

		m_GraphicsDevice.Shutdown();
	}
	
	// Camera�� �޾� Data(viewport)�� ����
	// clear, set�� �ٸ� ������?
	// Hazle�� �����ε� �Ǿ��ִ�.
	void Renderer::BeginScene()
	{
		auto pImmediateContext = m_GraphicsDevice.GetImmediateContext();
		auto pRenderTargetView = m_pSampleTex ? m_pSampleTex->GetRenderTargetView() : nullptr;
		auto pDepthStencilView = m_pDepthStencilTex ? m_pDepthStencilTex->GetDepthStencilView() : nullptr;
		if (!pImmediateContext || !pRenderTargetView)
			return;

		// active camera�κ��� �����;� �Ѵ�.
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

	// �� ������ ũ�� Ȯ�� �� ���� �� createRenderTargets()�� ȣ���ϴ� ���� ���� �� ����.
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
		// �̷� �ֵ��� ���� Ȯ���� ������.
		// ����ü Ȥ�� Ŭ����ȭ�ϱ⿣ �ʹ� ���ǰ� �۰�, �̸��� ������ ���̱⵵ �ָ��ϴ�.
		// �׷��ٸ� �ᱹ �� ��, Ȥ�� Graphics�� �����Լ����� ���� ���� �α׸� ����ؾ� �Ѵ�.
		// �׸��� �̴� ���ϼ��� �־�� �Ѵ�.
	}

	void Renderer::createDepthStencilStates()
	{
		
	}
	
	void Renderer::createRasterizerStates()
	{
		
	}
	
	// ������ üũ�ϴ°� �´� �� ����.
	// ���� �� �Լ��� void�� �ʿ�� ����.
	void Renderer::createShaders()
	{
		ID3D11PixelShader* pPixelShader = nullptr;
		if (!m_GraphicsDevice.CreatePixelShader("Assets/Shaders/none.hlsl", &pPixelShader))
		{
			DV_CORE_WARN("���� ����.");
			
		}
	}
}