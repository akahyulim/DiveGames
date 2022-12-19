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

	// urho의 Batch이며, 결국 이게 Draw Call이다.
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

		// view와 camera를 받는다.
		void Prepare(View* pView);
		// 역시 view와 camera를 받는다.
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
	// 인스턴싱에 사용될 데이터들을 관리하는 구조체
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
	// 인스턴싱 지오메트리를 관리하는 구조체
	struct BatchGroup : public Batch
	{
		// AddTransforms

		// SetInstancingData

		// Draw : override?

		std::vector<InstanceData> m_InstanceDatas;
	};

	// batch group key
	// Batch를 전달받아 각각의 요소들을 저장하고
	// 이들을 비교 및 hash를 생성에 활용한다.

	// Batch와 BatchGroup를 가지는 큐
	// 정렬함수와 draw 함수를 가지며,
	// draw 함수의 경우 Batch, BatchGroup의 것을 사용한다.
	struct BatchQueue
	{
		// clear
		// draw

		// sort - batckToFront, FrontToBack

		// SetInatancingData 역시 이 곳에 있다.

		std::vector<Batch> m_Batches;
		std::vector<Batch*> m_SortedBatches;
	};
}