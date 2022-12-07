#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Graphics/GraphicsDefs.h"
#include "Scene/Component/Drawable.h"

namespace Dive
{
	class View;
	class Pass;
	class ShaderVariation;

	// urho�� Batch�̸�, �ᱹ �̰� Draw Call�̴�.
	struct Batch
	{
		Batch() = default;
		Batch(const DrawableSourceData& data)
			: m_pMesh(data.m_pMesh),
			m_pMaterial(data.m_pMaterial),
			m_GeometryType(data.m_GeometryType)
		{
		}

		// view�� camera�� �޴´�.
		void Prepare(View* pView);
		// ���� view�� camera�� �޴´�.
		void Draw(View* pView);

		Mesh* m_pMesh = nullptr;
		Material* m_pMaterial = nullptr;
		Pass* m_pPass = nullptr;
		ShaderVariation* m_pVertexShader = nullptr;
		ShaderVariation* m_pPixelShader = nullptr;
		eGeometryType m_GeometryType = eGeometryType::Static;
	};

	// geometry instance data
	// �ν��Ͻ̿� ���� �����͵��� �����ϴ� ����ü
	struct InstanceData
	{
		InstanceData() = default;
		InstanceData(const void* pInstancingData, const DirectX::XMFLOAT4X4* worldTransform, float distance )
			: m_pInstancingData(pInstancingData),
			m_WorldTransform(worldTransform),
			m_Distance(distance)
		{}

		const void* m_pInstancingData = nullptr;
		const DirectX::XMFLOAT4X4* m_WorldTransform;
		float m_Distance = 0.0f;	// ī�޶���� �Ÿ�
	};

	// batch group
	// �ν��Ͻ� ������Ʈ���� �����ϴ� ����ü
	struct BatchGroup : public Batch
	{
		// AddTransforms

		// SetInstancingData

		// Draw : override?

		std::vector<InstanceData> m_InstanceDatas;
	};

	// batch group key
	// Batch�� ���޹޾� ������ ��ҵ��� �����ϰ�
	// �̵��� �� �� hash�� ������ Ȱ���Ѵ�.

	// Batch�� BatchGroup�� ������ ť
	// �����Լ��� draw �Լ��� ������,
	// draw �Լ��� ��� Batch, BatchGroup�� ���� ����Ѵ�.
	struct BatchQueue
	{
		// clear
		// draw

		// sort - batckToFront, FrontToBack

		// SetInatancingData ���� �� ���� �ִ�.

		std::vector<Batch> m_Batches;
		std::vector<Batch*> m_SortedBatches;
	};
}