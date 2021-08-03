#include "GBuffer.h"
#include "DiveCore.h"

// 그러고보니 SRV를 Shader에 전달하는 과정을 확인하지 못했다.
// RTV는 OMSetRenderTargets()로 세팅했다.
// => 예제를 살펴보니 Light에서 ID3D11ShaderResourceView* 배열로 묶은 후 PSSetShaderResources()로 전달함을 확인했다.
// 즉, 이 클래스는 RTV, DSV, SRV를 데이터 타입에 맞춰 생성하고 + @로 ConstantBuffer까지 만들었다.
namespace dive
{
	// Resize시에도 호출된다.
	bool GBuffer::Initialize(unsigned int width, unsigned int height)
	{
		Clear();

		// 직접 생성하므로 꽤 길다...


		// Constant Buffer까지 생성한다.

		return false;
	}
	
	void GBuffer::Clear()
	{
		DV_RELEASE(m_pTexDepthStencil);
		DV_RELEASE(m_pTexColorSpecIntensity);
		DV_RELEASE(m_pTexNormal);
		DV_RELEASE(m_pTexSpecPower);

		DV_RELEASE(m_pDepthStencilView);
		DV_RELEASE(m_pReadOnlyDepthStencilView);
		DV_RELEASE(m_pColorSpecIntensityRTV);
		DV_RELEASE(m_pNormalRTV);
		DV_RELEASE(m_pSpecPowerRTV);

		DV_RELEASE(m_pDepthStencilSRV);
		DV_RELEASE(m_pColorSpecIntensitySRV);
		DV_RELEASE(m_pNormalSRV);
		DV_RELEASE(m_pSpecPowerRTV);

		DV_RELEASE(m_pDepthStencilState);
	}
	
	void GBuffer::PreRender(ID3D11DeviceContext* pImmediateContext)
	{
		DV_ASSERT(pImmediateContext);

		// DepthStencilView
		pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// RenderTargetView
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		pImmediateContext->ClearRenderTargetView(m_pColorSpecIntensityRTV, clearColor);
		pImmediateContext->ClearRenderTargetView(m_pNormalRTV, clearColor);
		pImmediateContext->ClearRenderTargetView(m_pSpecPowerRTV, clearColor);

		ID3D11RenderTargetView* pRT[3] = { m_pColorSpecIntensityRTV, m_pNormalRTV, m_pSpecPowerRTV };
		pImmediateContext->OMSetRenderTargets(3, pRT, m_pDepthStencilView);

		pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
	}
	
	void GBuffer::PostRender(ID3D11DeviceContext* pImmediateContext)
	{
		DV_ASSERT(pImmediateContext);

		ID3D11RenderTargetView* pRT[3] = { nullptr, nullptr, nullptr };
		pImmediateContext->OMSetRenderTargets(3, pRT, m_pReadOnlyDepthStencilView);
	}
	
	void GBuffer::PrepareForUnpack(ID3D11DeviceContext* pImmediateContext)
	{
		DV_ASSERT(pImmediateContext);

		// ConstantBuffer를 map / unmap하여 투영 행렬과 뷰 역행렬을 계산한 후 저장한다.
		// 그리고 PS에 Set한다.
	}
}