#pragma once
#include "dvTexture.h"
#include "../Renderer.h"

namespace Dive
{
	// GBuffer용으로 테스트를 성공했다.
	// 카메라가 비추는 걸 텍스쳐에 따로 그린다.(카메라가 2개 이상인 이유)
	// 기본적으로 color와 depth(&stencil)을 모두 가지며
	// cube map으로 구성할 수 있고, mipmap 생성도 지원할 수 있어야 한다.
	// 즉, 다양한 형태로 사용할 수 있는데, 이를 어떻게 설정하고 생성하느냐가 중요하다.
	// 참고1: https://docs.unity3d.com/kr/530/ScriptReference/RenderTexture.html
	// 참고2: https://docs.unity3d.com/kr/2019.4/Manual/class-RenderTexture.html
	// 참고3: https://www.rastertek.com/dx11tut11.html
	// 참고4: https://www.rastertek.com/dx11tut22.html

	// 유니티의 경우에도 Create로 리소스를 생성한다.
	// 다만 기본적으로는 active로 설정된 시점에 생성하고, 이 경우엔 Create가 아무 작업도 하지 않는다.
	class dvRenderTexture : public dvTexture
	{
	public:
		// 이렇게 생성자로 버퍼까지 생성해버리면 추후 Resize류 함수가 필요할 수 밖에 없게 된다.
		// 기타 인자가 꽤 많기 때문이다.
		dvRenderTexture(unsigned int width, unsigned int height, unsigned int depth = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
		{
			m_Width = width;
			m_Height = height;
			m_Depth = depth;
			m_Format = format;

			m_bCreated = false;
		}

		// 유니티 interface
		bool Create();
		void Release();
		bool IsCreated() const { return m_bCreated; }

		ID3D11RenderTargetView* GetColorRenderTargetView() { return m_pColorRenderTargetView.Get(); }
		ID3D11ShaderResourceView* GetColorShaderResourceView() { return m_pColorShaderResourceView.Get(); }

		ID3D11DepthStencilView* GetDepthStencilView() { return m_pDepthStencilView.Get(); }
		ID3D11ShaderResourceView* GetDepthShaderResourceView() { return m_pDepthShaderResourceView.Get(); }

		// 기타 변수 Get & Set
		static dvRenderTexture* GetActive() { return m_pActive; }
		static void SetActive(dvRenderTexture* pRenderTexture);

	private:
		bool createColorResource();
		bool createDepthStencilResource();

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

		bool m_bCreated = false;

		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
	
		unsigned int m_Depth = 0;	// 0, 16(depth only), 24	24라는건 depth 24, stencil 8을 의미 할거다.

		//bool m_bGenerateMips = false;	// 자동, 매뉴얼 생성 여부이다...
		bool m_bUseMipMap = false;		// 이건 밉맵 생성 여부이다.

		unsigned int m_AntiAliasing = 1; // 1, 2, 4, 8로 나뉜다.

		// 뭔지 아직 모름...
		static dvRenderTexture* m_pActive;
	};
}