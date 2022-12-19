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
		Batch()
			: m_pMesh(nullptr),
			m_pMaterial(nullptr),
			m_pWorldTransform(nullptr),
			m_pPass(nullptr),
			m_pVertexShaderVariation(nullptr),
			m_pPixelShaderVariation(nullptr),
			m_GeometryType(eGeometryType::Static)
		{}

		Batch(const DrawableSourceData& data)
			: m_pMesh(data.pMesh),
			m_pMaterial(data.pMaterial),
			m_pWorldTransform(data.pWorldTransform),
			m_pPass(nullptr),
			m_pVertexShaderVariation(nullptr),
			m_pPixelShaderVariation(nullptr), 
			m_GeometryType(data.GeometryType)
		{}

		// view�� camera�� �޴´�.
		void Prepare(View* pView);
		// ���� view�� camera�� �޴´�.
		void Draw(View* pView);

		Mesh* m_pMesh;
		Material* m_pMaterial;
		const DirectX::XMFLOAT4X4* m_pWorldTransform;
		Pass* m_pPass;
		ShaderVariation* m_pVertexShaderVariation;
		ShaderVariation* m_pPixelShaderVariation;
		eGeometryType m_GeometryType;
	};

	// geometry instance data
	// �ν��Ͻ̿� ���� �����͵��� �����ϴ� ����ü
	struct InstanceData
	{
		InstanceData()
			: m_pInstancingData(nullptr),
			m_pWorldTransform(nullptr),
			m_Distance(0.0f)
		{}

		InstanceData(const void* pInstancingData, const DirectX::XMFLOAT4X4* pWorldTransform, float distance )
			: m_pInstancingData(pInstancingData),
			m_pWorldTransform(pWorldTransform),
			m_Distance(distance)
		{}

		const void* m_pInstancingData;
		const DirectX::XMFLOAT4X4* m_pWorldTransform;
		float m_Distance;
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