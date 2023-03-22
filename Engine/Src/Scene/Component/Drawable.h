#pragma once
#include "Component.h"
#include "Graphics/GraphicsDefs.h"

namespace Dive
{
	class Context;
	class Mesh;
	class Model;
	class Material;
	class Light;
	
	struct DrawableSourceData
	{
		eGeometryType GeometryType;
		Mesh* pMesh;
		Material* pMaterial;
		DirectX::XMFLOAT4X4 WorldTransform;
		float Distance;
	};

	class Drawable : public Component
	{
		DIVE_OBJECT(Drawable, Component)

	public:
		explicit Drawable(Context* pContext);
		~Drawable() override;

		void Update(float delta) override;

		Model* GetModel() const { return m_pModel; }
		virtual void SetModel(Model* pModel);

		virtual Material* GetMaterial() const { return GetMaterial(0); }
		virtual void SetMaterial(Material* material);
		virtual Material* GetMaterial(size_t index) const;
		virtual bool SetMaterial(size_t index, Material* material);

		uint32_t GetMeshCount() const { return static_cast<uint32_t>(m_Meshes.size()); }
		void SetNumMesh(uint32_t num);

		Mesh* GetMesh(size_t batchIndex);

		const std::vector<Light*>& GetLights() const { return m_Lights; }
		void AddLight(Light* pLight);

		const std::vector<DrawableSourceData>& GetSourceDatas() const { return m_SourceDatas; }

	protected:
		
	protected:
		Model* m_pModel;

		std::vector<Mesh*> m_Meshes;
		std::vector<DrawableSourceData> m_SourceDatas;

		float m_Distance;

		// urho는 vertex, pixel을 나누었다.
		std::vector<Light*> m_Lights;

		// 전체 메시 정보에서 부분을 그리는 geometry다.
		// vertex와 index의 offset, count를 관리한다.
		// material도 관리한다.
		// model은 resource이지만
		// 이를 포함한 gameobject는 어떻게 관리해야 할 지 생각해야 한다.
		// 그리고 현재 구현 상태에 많은 변화를 수반할 것이다.
		// instancing과 유니티의 구현 상태 그리고 SkinnedMesh 확장도 염두해야 한다.
	};
}