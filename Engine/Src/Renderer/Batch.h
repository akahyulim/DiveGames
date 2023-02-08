#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Graphics/GraphicsDefs.h"
#include "Scene/Component/Drawable.h"

namespace Dive
{
	class View;
	class Camera;
	class Pass;
	class ShaderVariation;

	// urho�� Batch�̸�, �ᱹ �̰� Draw Call�̴�.
	// �̸��� DrawCall�� �ٲٴ� ���� �����ص���.
	// => StaticBatch�� ���� �� ����.
	class Batch
	{
	public:
		Batch() = default;
		Batch(const DrawableSourceData& data);
		
		void Prepare(View* pView, Camera* pCamera);
		
		void Draw(View* pView, Camera* pCamera);

		Mesh* GetMesh() const { return m_pMesh; }
		
		Material* GetMaterial() const { return m_pMaterial; }
		void SetMaterial(Material* pMaterial) { m_pMaterial = pMaterial; }
		
		DirectX::XMFLOAT4X4 GetWorldTransform() const { return m_WorldTransform; }

		Pass* GetPass() const { return m_pPass; }
		void SetPass(Pass* pPass) { m_pPass = pPass; }

		ShaderVariation* GetVertexShaderVariation() const { return m_pVertexShaderVariation; }
		void SetVertexShaderVariation(ShaderVariation* pVariation) { m_pVertexShaderVariation = pVariation; }

		ShaderVariation* GetPiexelShaderVariation() const { return m_pPixelShaderVariation; }
		void SetPixelShaderVariation(ShaderVariation* pVariation) { m_pPixelShaderVariation = pVariation; }

		eGeometryType GetGeometryType() const { return m_GeometryType; }
		void SetGeometryType(eGeometryType type) { m_GeometryType = type; }

	protected:
		//float Distance;
		Mesh* m_pMesh;
		Material* m_pMaterial;
		DirectX::XMFLOAT4X4 m_WorldTransform;
		Pass* m_pPass;
		// ��� ShaderVariation���� Pass���� �̹� �����Ѵ�.
		ShaderVariation* m_pVertexShaderVariation;
		ShaderVariation* m_pPixelShaderVariation;
		eGeometryType m_GeometryType;
	};

	// geometry instance data
	// �ν��Ͻ̿� ���� �����͵��� �����ϴ� ����ü
	struct InstanceData
	{
		// const��� �����ڿ��� �޾ƾ� �Ѵ�. ���� Ŭ������ ������ �Ѵ�?
		// => ������ ����ü �Ӹ� �ƴ϶� Ŭ�������� const �׸��� ���� ��� ������ ������ ����� �Ѵ�.
		void* pInstancingData;
		DirectX::XMFLOAT4X4* pWorldTransform;
		float Distance;
	};

	// batch group
	// �ν��Ͻ� ������Ʈ���� �����ϴ� ����ü
	class InstancingBatch : public Batch
	{
	public:
		void AddTransforms(const Batch& batch);
		
		void SetInstancingData(void* pLockedData, uint32_t stride, uint32_t& freeIndex);

		void Draw(View* pView, Camera* pCamera);

	private:
		std::vector<InstanceData> m_InstanceDatas;
		uint32_t m_StartIndex;	// �̰� �ִ밪���� �ʱ�ȭ�Ѵ�.
	};

	// batch group key
	// Batch�� ���޹޾� ������ ��ҵ��� �����ϰ�
	// �̵��� �� �� hash�� ������ Ȱ���Ѵ�.

	// Batch�� BatchGroup�� ������ ť
	// �����Լ��� draw �Լ��� ������,
	// draw �Լ��� ��� Batch, BatchGroup�� ���� ����Ѵ�.
	// => Pope�� Struct�� �Լ��� ���� ����� �Ѵ�.
	class BatchQueue
	{
	public:
		BatchQueue() = default;
		~BatchQueue() = default;

		void Clear();

		void Draw(View* pView, Camera* pCamera) const;

		void AddStaticBatch(const Batch& batch) { m_Batches.emplace_back(batch); }
		void AddInstancingBatch(const InstancingBatch& batch) { m_InstancingBatches.emplace_back(batch); }

		void SortBackToFront();
		void SortFrontToBack();

		void SetInstancingData(void* pLockedData, uint32_t stride, uint32_t& freeIndex);
		
		bool IsEmpty() const { return m_Batches.empty() && m_InstancingBatches.empty(); }

	private:
		std::vector<Batch> m_Batches;
		std::vector<Batch*> m_SortedBatches;

		std::vector<InstancingBatch> m_InstancingBatches;	//key�� ������ map�̾�� �Ѵ�.
		std::vector<InstancingBatch*> m_SortedInstancingBatches;
	};
}