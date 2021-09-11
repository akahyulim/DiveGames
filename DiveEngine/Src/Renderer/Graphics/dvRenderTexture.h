#pragma once
#include "dvTexture.h"
#include "../Renderer.h"

namespace dive
{
	// 유니티를 따라가는게 나을 것 같다.
	// 결국 이건 rasterTek의 2D Rednering과 동일하다.
	// 카메라가 비추는 걸 텍스쳐에 따로 그린다.(카메라가 2개 이상인 이유)
	// 기본적으로 color와 depth(&stencil)을 모두 가지며
	// cube map으로 구성할 수 있고, mipmap 생성도 지원할 수 있어야 한다.
	// 참고1: https://docs.unity3d.com/kr/530/ScriptReference/RenderTexture.html
	// 참고2: https://docs.unity3d.com/kr/2019.4/Manual/class-RenderTexture.html
	// 참고3: https://www.rastertek.com/dx11tut11.html
	// 참고4: https://www.rastertek.com/dx11tut22.html
	class dvRenderTexture : public dvTexture
	{
	public:
		// 유니티의 생성자를 참고했다. 하지만 이걸로 퉁칠 순 없다.
		// dvTexture의 멤버들을 Set할 수 있고, 생성자에 포함되지 않은 멤버 변수도 Set할 수 있다.
		// 그렇다면 Create()를 따로 만들어야 할까? 아님 Update에서 DirtyCheck를 한 후 새로 만들어야 할까? 
		// DirtyCheck는 App에선 에바 같다. 완전히 설정된 텍스처를 만들어야 하지 않을까? 
		// 하지만 태생적으로 생성자만으로는 불가능하다. 그렇다면 자체 포멧 로드...?
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
					desc.MiscFlags = 0;	// mipmap generate를 생각해야 한다.

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
					// cubemap이면 array 였던가...
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

		// 유니티는 ColorBuffer, DepthBuffer로 구분한다.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pColorResource;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pColorRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pColorShaderResourceView;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilResource;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pDepthShaderResourceView;

		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
	
		unsigned int m_Depth = 0;	// 0, 16(depth only), 24	24라는건 depth 24, stencil 8을 의미 할거다.

		//bool m_bGenerateMips = false;	// 자동, 매뉴얼 생성 여부이다...
		bool m_bUseMipMap = false;		// 이건 밉맵 생성 여부이다.

		unsigned int m_AntiAliasing = 1; // 1, 2, 4, 8로 나뉜다.

		// 뭔지 아직 모름...
		static dvRenderTexture* m_pActive;
	};
}