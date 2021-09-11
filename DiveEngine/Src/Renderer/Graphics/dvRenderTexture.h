#pragma once
#include "dvTexture.h"
#include "../Renderer.h"

namespace dive
{
	// ����Ƽ�� ���󰡴°� ���� �� ����.
	// �ᱹ �̰� rasterTek�� 2D Rednering�� �����ϴ�.
	// ī�޶� ���ߴ� �� �ؽ��Ŀ� ���� �׸���.(ī�޶� 2�� �̻��� ����)
	// �⺻������ color�� depth(&stencil)�� ��� ������
	// cube map���� ������ �� �ְ�, mipmap ������ ������ �� �־�� �Ѵ�.
	// ����1: https://docs.unity3d.com/kr/530/ScriptReference/RenderTexture.html
	// ����2: https://docs.unity3d.com/kr/2019.4/Manual/class-RenderTexture.html
	// ����3: https://www.rastertek.com/dx11tut11.html
	// ����4: https://www.rastertek.com/dx11tut22.html
	class dvRenderTexture : public dvTexture
	{
	public:
		// ����Ƽ�� �����ڸ� �����ߴ�. ������ �̰ɷ� ��ĥ �� ����.
		// dvTexture�� ������� Set�� �� �ְ�, �����ڿ� ���Ե��� ���� ��� ������ Set�� �� �ִ�.
		// �׷��ٸ� Create()�� ���� ������ �ұ�? �ƴ� Update���� DirtyCheck�� �� �� ���� ������ �ұ�? 
		// DirtyCheck�� App���� ���� ����. ������ ������ �ؽ�ó�� ������ ���� ������? 
		// ������ �»������� �����ڸ����δ� �Ұ����ϴ�. �׷��ٸ� ��ü ���� �ε�...?
		dvRenderTexture(unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
		{
			m_pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
			m_pDeviceContext = Renderer::GetInstance().GetGraphicsDevice()->GetImmediateContext();

			m_Width = width;
			m_Height = height;
			m_Depth = depth;
			m_Format = format;

			// color buffer
			{
				// resource
				{
					D3D11_TEXTURE2D_DESC desc; 
					ZeroMemory(&desc, sizeof(desc));
					desc.Width = m_Width;
					desc.Height = m_Height;
					desc.ArraySize = 1;
					desc.MipLevels = 1;
					desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
					desc.Format = m_Format;
					desc.Usage = D3D11_USAGE_DEFAULT;// D3D11_USAGE_DYNAMIC;
					desc.SampleDesc.Count = 1;
					desc.SampleDesc.Quality = 0;
					desc.CPUAccessFlags = 0;
					desc.MiscFlags = 0;	// mipmap generate�� �����ؾ� �Ѵ�.

					if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pColorResource.GetAddressOf())))
					{
						CORE_ERROR("");
						return;
					}
				}

				// srv
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC desc;
					ZeroMemory(&desc, sizeof(desc));
					desc.Format = m_Format;
					// cubemap�̸� array ������...
					desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					desc.Texture2D.MostDetailedMip = 0;
					desc.Texture2D.MipLevels = -1;

					if (FAILED(m_pDevice->CreateShaderResourceView(m_pColorResource.Get(), &desc, m_pColorShaderResourceView.GetAddressOf())))
					{
						CORE_ERROR("");
						return;
					}
				}

				// rtv
				{
					D3D11_RENDER_TARGET_VIEW_DESC desc;
					ZeroMemory(&desc, sizeof(desc));
					desc.Format = m_Format;

					if(FAILED(m_pDevice->CreateRenderTargetView(m_pColorResource.Get(), &desc, m_pColorRenderTargetView.GetAddressOf())))
					{
						CORE_ERROR("");
						return;
					}
				}
			}

			// depth buffer
			{
				if (m_Depth == 0)
					return;

				// resource
				{
					D3D11_TEXTURE2D_DESC desc;
					ZeroMemory(&desc, sizeof(desc));
					desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
					desc.Format = m_Depth == 16 ? DXGI_FORMAT_R16_TYPELESS : DXGI_FORMAT_R24G8_TYPELESS;

					if(FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pDepthStencilResource.GetAddressOf())))
					{
						CORE_ERROR("");
						return;
					}
				}

				// srv
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC desc;
					ZeroMemory(&desc, sizeof(desc));
					desc.Format = m_Depth == 16 ? DXGI_FORMAT_R16_UNORM : DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

					if(FAILED(m_pDevice->CreateShaderResourceView(m_pDepthStencilResource.Get(), &desc, m_pDepthShaderResourceView.GetAddressOf())))
					{
						CORE_ERROR("");
						return;
					}
				}

				// dsv
				{
					D3D11_DEPTH_STENCIL_VIEW_DESC desc;
					ZeroMemory(&desc, sizeof(desc));
					desc.Format = m_Depth == 16 ? DXGI_FORMAT_D16_UNORM : DXGI_FORMAT_D24_UNORM_S8_UINT;

					if(FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilResource.Get(), &desc, m_pDepthStencilView.GetAddressOf())))
					{
						CORE_ERROR("");
						return;
					}
				}
			}
		}

	private:

	private:
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pDeviceContext = nullptr;

		// ����Ƽ�� ColorBuffer, DepthBuffer�� �����Ѵ�.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pColorResource;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pColorRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pColorShaderResourceView;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilResource;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pDepthShaderResourceView;

		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
	
		unsigned int m_Depth = 0;	// 0, 16(depth only), 24	24��°� depth 24, stencil 8�� �ǹ� �ҰŴ�.

		//bool m_bGenerateMips = false;	// �ڵ�, �Ŵ��� ���� �����̴�...
		bool m_bUseMipMap = false;		// �̰� �Ӹ� ���� �����̴�.

		unsigned int m_AntiAliasing = 1; // 1, 2, 4, 8�� ������.

		// ���� ���� ��...
		static dvRenderTexture* m_pActive;
	};
}