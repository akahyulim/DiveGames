#include "DivePch.h"
#include "SkinnedMeshRenderer.h"
#include "Transform.h"
#include "Scene/GameObject.h"
#include "Renderer/Renderer.h"
#include "Renderer/Material.h"
#include "Renderer/SkinnedMesh.h"
#include "Graphics/Graphics.h"
#include "Graphics/ConstantBuffer.h"
#include "IO/Log.h"

#include "Renderer/Animator.h"

namespace Dive
{
	SkinnedMeshRenderer::SkinnedMeshRenderer(GameObject* pGameObject)
		: IMeshRenderer(pGameObject),
		m_pAnimator(nullptr)
	{
		m_pAnimator = new Animator;
	}

	SkinnedMeshRenderer::~SkinnedMeshRenderer()
	{
		DV_CORE_TRACE("Destroy SkinnedMeshRenderer: {:s}", GetName());
	}

	void SkinnedMeshRenderer::Draw() const
	{
		// Material
		{
			Graphics::SetTexture(eTextureUnit::Albedo, m_pMaterial->GetTexture(eTextureUnit::Albedo));

			// material pixel shader buffer
			{
				auto pBuffer = Renderer::GetMaterialPixelShaderBuffer();
				auto pMappedData = static_cast<MaterialPixelShaderBuffer*>(pBuffer->Map());
				pMappedData->albedoColor = m_pMaterial->GetAlbedoColor();
				pMappedData->tiling = m_pMaterial->GetTiling();
				pMappedData->offset = m_pMaterial->GetOffset();
				pBuffer->Unmap();
				Graphics::SetConstantBuffer(2, eShaderType::PixelShader, pBuffer);
			}

			// 일단 셰이더를 별도로 만들어 사용했다.
			// 렌더링엔 성공했다.
			Graphics::SetShaderVariation(eShaderType::VertexShader, Renderer::GetBasicSkinnedVertexShaderVariation());
			Graphics::SetShaderVariation(eShaderType::PixelShader, Renderer::GetBasicSkinnedPixelShaderVariation());
		}

		// Mesh Draw
		{
			auto pBuffer = Renderer::GetModelVertexShaderBuffer();
			auto pMappedData = static_cast<ModelVertexShaderBuffer*>(pBuffer->Map());
			pMappedData->worldMatrix = DirectX::XMMatrixTranspose(m_pGameObject->GetTransform()->GetMatrix());
			{
				//auto boneTransforms = m_pAnimator->GetFinalBoneMatrices();
				//for (size_t i = 0; i < boneTransforms.size(); ++i)
				{
				//	DirectX::XMFLOAT4X4 boneFinal = boneTransforms[i];
				//	pMappedData->skinMatrix[i] = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&boneFinal));
				}
			}
			pBuffer->Unmap();
			Graphics::SetConstantBuffer(1, eShaderType::VertexShader, pBuffer);

			Graphics::SetVertexBuffer(m_pMesh->GetVertexBuffer());
			Graphics::SetIndexBuffer(m_pMesh->GetIndexBuffer());
			Graphics::DrawIndexed(
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				m_pMesh->GetNumIndices(),
				0, 0);
		}
	}

	IMesh* SkinnedMeshRenderer::GetMesh()
	{
		return m_pMesh ? static_cast<IMesh*>(m_pMesh) : nullptr;
	}
	
	void SkinnedMeshRenderer::SetMesh(IMesh* pMesh)
	{
		m_pMesh = dynamic_cast<SkinnedMesh*>(pMesh);
	}

	void SkinnedMeshRenderer::SetAnimation(Animation* pAnim)
	{
		if (!m_pAnimator)
			return;

		m_pAnimator->SetAnimation(pAnim);
	}
}