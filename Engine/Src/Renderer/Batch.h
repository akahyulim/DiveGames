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
		Batch() = default;
		Batch(const DrawableSourceData& data)
			: m_pMesh(data.m_pMesh),
			m_pMaterial(data.m_pMaterial),
			m_GeometryType(data.m_GeometryType)
		{
		}

		// view와 camera를 받는다.
		void Prepare(View* pView);
		// 역시 view와 camera를 받는다.
		void Draw(View* pView);

		Mesh* m_pMesh = nullptr;
		Material* m_pMaterial = nullptr;
		Pass* m_pPass = nullptr;
		ShaderVariation* m_pVertexShader = nullptr;
		ShaderVariation* m_pPixelShader = nullptr;
		eGeometryType m_GeometryType = eGeometryType::Static;
	};

	// geometry instance data
	// 인스턴싱에 사용될 데이터들을 관리하는 구조체
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
		float m_Distance = 0.0f;	// 카메라와의 거리
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