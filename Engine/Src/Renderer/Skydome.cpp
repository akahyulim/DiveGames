#include "divepch.h"
#include "Skydome.h"
#include "MeshFactory.h"
#include "Renderer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Texture2D.h"
#include "Graphics/ConstantBuffer.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	Skydome::Skydome()
		: m_pMesh(nullptr)
		, m_ApexColor(0.0f, 0.05f, 0.6f, 1.0f)
		, m_CenterColor(0.0f, 0.5f, 0.8f, 1.0f)
		, m_pTexture(nullptr)
		, m_pConstantBuffer(nullptr)
		, m_pVertexShader(nullptr)
		, m_pPixelShader(nullptr)
	{
		m_pMesh = CreateSphere();

		m_pConstantBuffer = new ConstantBuffer("SkydomeConstantBuffer");
		m_pConstantBuffer->Create<SkydomeConstantBuffer>();
		
		m_pVertexShader = Renderer::GetSkydomeShaderVariation(eShaderType::VertexShader);
		m_pPixelShader = Renderer::GetSkydomeShaderVariation(eShaderType::PixelShader);
	}

	Skydome::~Skydome()
	{
		DV_DELETE(m_pConstantBuffer);
		DV_DELETE(m_pMesh);
	}
	
	void Skydome::Update()
	{
	}
	
	void Skydome::Render()
	{
		// cull & depth off
		Graphics::SetDepthStencilState(Renderer::GetDepthStencilState(eDepthStencilState::DepthDiabled), 1);
		Graphics::GetDeviceContext()->RSSetState(Renderer::GetRasterizerState(eRasterizerState::FillSolid_CullNone));

		// matrial
		Graphics::SetTexture(eTextureUnit::Diffuse, m_pTexture);
		// �������� �������� ����ϴ� ���۴� Ÿ���� ���� �� �ִ�.
		auto pMappedData = static_cast<SkydomeConstantBuffer*>(m_pConstantBuffer->Map());
		pMappedData->apexColor = m_ApexColor;
		pMappedData->centerColor = m_CenterColor;
		m_pConstantBuffer->Unmap();
		Graphics::SetConstantBuffer(0, eShaderType::PixelShader, m_pConstantBuffer);

		// shader
		Graphics::SetShaderVariation(eShaderType::VertexShader, m_pVertexShader);
		Graphics::SetShaderVariation(eShaderType::PixelShader, m_pPixelShader);

		// buffer
		// Ȥ�� �޽ÿ��� ���� ȣ��...? ����Ű�� �޽ÿ��� ��Ƽ������� ���ε��ߴ�.
		Graphics::SetVertexBuffer(m_pMesh->GetVertexBuffer());
		Graphics::SetIndexBuffer(m_pMesh->GetIndexBuffer());

		// draw
		Graphics::DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_pMesh->GetNumIndices(), 0);
	}
}