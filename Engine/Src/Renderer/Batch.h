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

	// urho의 Batch이며, 결국 이게 Draw Call이다.
	// 이름을 DrawCall로 바꾸는 것을 염두해두자.
	// => StaticBatch가 나을 것 같다.
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
		// 사실 ShaderVariation들은 Pass에도 이미 존재한다.
		ShaderVariation* m_pVertexShaderVariation;
		ShaderVariation* m_pPixelShaderVariation;
		eGeometryType m_GeometryType;
	};

	// geometry instance data
	// 인스턴싱에 사용될 데이터들을 관리하는 구조체
	struct InstanceData
	{
		// const라면 생성자에서 받아야 한다. 따라서 클래스로 만들어야 한다?
		// => 포프는 구조체 뿐만 아니라 클래스에도 const 그리고 참조 멤버 변수를 만들지 마라고 한다.
		void* pInstancingData;
		DirectX::XMFLOAT4X4* pWorldTransform;
		float Distance;
	};

	// batch group
	// 인스턴싱 지오메트리를 관리하는 구조체
	class InstancingBatch : public Batch
	{
	public:
		void AddTransforms(const Batch& batch);
		
		void SetInstancingData(void* pLockedData, uint32_t stride, uint32_t& freeIndex);

		void Draw(View* pView, Camera* pCamera);

	private:
		std::vector<InstanceData> m_InstanceDatas;
		uint32_t m_StartIndex;	// 이건 최대값으로 초기화한다.
	};

	// batch group key
	// Batch를 전달받아 각각의 요소들을 저장하고
	// 이들을 비교 및 hash를 생성에 활용한다.

	// Batch와 BatchGroup를 가지는 큐
	// 정렬함수와 draw 함수를 가지며,
	// draw 함수의 경우 Batch, BatchGroup의 것을 사용한다.
	// => Pope는 Struct에 함수를 넣지 마라고 한다.
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

		std::vector<InstancingBatch> m_InstancingBatches;	//key를 가지는 map이어야 한다.
		std::vector<InstancingBatch*> m_SortedInstancingBatches;
	};
}