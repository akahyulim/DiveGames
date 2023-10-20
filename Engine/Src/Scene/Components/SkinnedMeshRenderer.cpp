#include "DivePch.h"
#include "SkinnedMeshRenderer.h"
#include "Transform.h"
#include "Animator.h"
#include "Core/Timer.h"
#include "Scene/GameObject.h"
#include "Renderer/Renderer.h"
#include "Renderer/Material.h"
#include "Renderer/SkinnedMesh.h"
#include "Renderer/Animation.h"
#include "Renderer/Bone.h"
#include "Graphics/Graphics.h"
#include "Graphics/ConstantBuffer.h"
#include "Core/Log.h"

#include "Renderer/Skeleton.h"
#include "BoneRenderer.h"

namespace Dive
{
	SkinnedMeshRenderer::SkinnedMeshRenderer(GameObject* pGameObject)
		: IMeshRenderer(pGameObject),
		m_CurrentTime(0.0f),
		m_pRootBone(nullptr)
	{
		DirectX::XMFLOAT4X4 matIdentity;
		DirectX::XMStoreFloat4x4(&matIdentity, DirectX::XMMatrixIdentity());
		m_SkinMatrices.reserve(100);
		for (uint32_t i = 0; i < 100; ++i)
			m_SkinMatrices.emplace_back(matIdentity);
	}

	SkinnedMeshRenderer::~SkinnedMeshRenderer()
	{
	}

	void SkinnedMeshRenderer::Update()
	{
		
	}

	void SkinnedMeshRenderer::Draw()
	{
		// Material
		{
			Graphics::SetTexture(eTextureUnit::Diffuse, m_pMaterial->GetTexture(eTextureUnit::Diffuse));

			// material pixel shader buffer
			{
				auto pBuffer = Renderer::GetMaterialPixelShaderBuffer();
				auto pMappedData = static_cast<MaterialPixelShaderBuffer*>(pBuffer->Map());
				pMappedData->diffuseColor = m_pMaterial->GetDiffuseColor();
				pMappedData->tiling = m_pMaterial->GetTiling();
				pMappedData->offset = m_pMaterial->GetOffset();
				pBuffer->Unmap();
				Graphics::SetConstantBuffer(2, eShaderType::PixelShader, pBuffer);
			}

			// �ϴ� ���̴��� ������ ����� ����ߴ�.
			// �������� �����ߴ�.
			Graphics::SetShaderVariation(eShaderType::VertexShader, Renderer::GetDeferredSkinnedShadingVertexShaderVariation());//GetBasicSkinnedVertexShaderVariation());
			Graphics::SetShaderVariation(eShaderType::PixelShader, Renderer::GetDeferredSkinnedShadingPixelShaderVariation());//GetBasicSkinnedPixelShaderVariation());
		}

		// �̰� update�� �־������ ������ ���� ���� ����ȴ�. ���� �׳� �� ���� �׷��� �Ѵ�.
		{
			float deltaTime = static_cast<float>(Dive::Timer::GetDeltaTimeSec());

			m_pCurrentAnimation = m_pGameObject->GetComponent<Dive::Animator>()->GetCurrentAnimation();
			if (m_pCurrentAnimation)
			{
				m_CurrentTime += m_pCurrentAnimation->GetTickPerSecond() * deltaTime;
				m_CurrentTime = fmod(m_CurrentTime, m_pCurrentAnimation->GetDuration());

				if (!m_Bones.empty() && m_pRootBone)
				{
					DirectX::XMFLOAT4X4 matIdentity;
					DirectX::XMStoreFloat4x4(&matIdentity, DirectX::XMMatrixIdentity());
					calcuBoneTransform(m_pRootBone->GetTransform(), matIdentity);
				}
			}

			auto pBuffer = Renderer::GetModelVertexShaderBuffer();
			auto pMappedData = static_cast<ModelVertexShaderBuffer*>(pBuffer->Map());
			pMappedData->worldMatrix = DirectX::XMMatrixTranspose(m_pGameObject->GetTransform()->GetMatrix());
			// 1. XMMatrixTranspose()������ memcpy ������ ������.
			// 2. XMMATRIX�� XMFLOAT4�� ��ȯ�� �ɸ��µ�, calcuBoneTransform()���� ����Ҷ� ������ Ÿ���� ���÷� �ٲ��.
			for (uint32_t i = 0; i < static_cast<uint32_t>(m_SkinMatrices.size()); ++i)
				pMappedData->skinMatrix[i] = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_SkinMatrices[i]));
			pBuffer->Unmap();
			Graphics::SetConstantBuffer(1, eShaderType::VertexShader, pBuffer);
		}

		// Mesh Draw
		{
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

	void SkinnedMeshRenderer::InsertBone(BoneInfo* pBone)
	{
		m_Bones.emplace_back(*pBone);
	}

	void SkinnedMeshRenderer::SetBones(const std::vector<BoneInfo>& bones)
	{
		m_Bones.clear();
		m_BoneInfoMap.clear();

		m_Bones = bones;

		// �ӽ� �����̴�. �����δ� �ϼ��� �� �޾ƿ��� ���� ���ƺ��δ�.
		for (const auto& bone : m_Bones)
		{
			m_BoneInfoMap[bone.name] = bone;
		}
	}

	void SkinnedMeshRenderer::calcuBoneTransform(Transform* pNode, DirectX::XMFLOAT4X4 parent)
	{
		auto nodeName = pNode->GetName();
		auto localTransform = pNode->GetLocalMatrix();
		
		auto pBone = m_pCurrentAnimation->FindBone(nodeName);
		if (pBone)
		{
			pBone->Update(m_CurrentTime); 
			localTransform = pBone->GetLocalTransform();
		}

		auto worldTransform = localTransform * DirectX::XMLoadFloat4x4(&parent);

		auto it = m_BoneInfoMap.find(nodeName);
		if (it != m_BoneInfoMap.end())
		{
			auto finalBoneTransform = DirectX::XMLoadFloat4x4(&it->second.offsetMatrix) * worldTransform;
			DirectX::XMStoreFloat4x4(&m_SkinMatrices[it->second.index], finalBoneTransform);
		}

		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, worldTransform);
		for (auto pChild : pNode->GetChildren())
			calcuBoneTransform(pChild, world);
	}
}