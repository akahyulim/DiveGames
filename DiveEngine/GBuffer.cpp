#include "GBuffer.h"
#include "DiveCore.h"

// �׷����� SRV�� Shader�� �����ϴ� ������ Ȯ������ ���ߴ�.
// RTV�� OMSetRenderTargets()�� �����ߴ�.
// => ������ ���캸�� Light���� ID3D11ShaderResourceView* �迭�� ���� �� PSSetShaderResources()�� �������� Ȯ���ߴ�.
// ��, �� Ŭ������ RTV, DSV, SRV�� ������ Ÿ�Կ� ���� �����ϰ� + @�� ConstantBuffer���� �������.
namespace dive
{
	// Resize�ÿ��� ȣ��ȴ�.
	bool GBuffer::Initialize(unsigned int width, unsigned int height)
	{
		Clear();

		// ���� �����ϹǷ� �� ���...


		// Constant Buffer���� �����Ѵ�.

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

		// ConstantBuffer�� map / unmap�Ͽ� ���� ��İ� �� ������� ����� �� �����Ѵ�.
		// �׸��� PS�� Set�Ѵ�.
	}
}