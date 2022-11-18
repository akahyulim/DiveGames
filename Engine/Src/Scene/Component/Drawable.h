#pragma once
#include "Component.h"

namespace Dive
{
	class Context;
	class Mesh;
	class Model;
	class Material;

	// 추후 GraphicsDef로 이동
	enum class eMeshType
	{
		Static,
		Skinned,
		Instanced,
		Billboard,
	};
	
	// 하나의 모델이 다수의 메시와 머티리얼로 구성될 수 있기에
	// 이들을 개별로 묶어 관리할 필요가 있다.
	struct DrawableBatch
	{
		DrawableBatch() = default;
		DrawableBatch(const DrawableBatch& rhs) = default;
		~DrawableBatch() = default;

		DrawableBatch& operator=(const DrawableBatch& rhs) = default;

		float m_Distance = 0.0f;
		Mesh* m_pMesh = nullptr;
		Material* m_pMaterial = nullptr;
		DirectX::XMFLOAT4X4 m_WorldTransform;	// skinned 용이라고 한다...
		eMeshType m_MeshType;
	};

	class Drawable : public Component
	{
		DIVE_OBJECT(Drawable, Component)

	public:
		explicit Drawable(Context* pContext);
		~Drawable() override;

		// virtual update => StaticDrawble까진 미구현이다. 게다가 worker thread가 호출한다고...
		// virtual updateBatches
		// virtual updateMeshes => StaticDrawble까진 미구현이다.

		Model* GetModel() const { return m_pModel; }
		virtual void SetModel(Model* pModel);

		virtual Material* GetMaterial() const { return GetMaterial(0); }
		virtual void SetMaterial(Material* pMaterial);
		virtual Material* GetMaterial(size_t index) const;
		virtual bool SetMaterial(size_t index, Material* pMaterial);

		unsigned int GetNumMesh() const { return static_cast<unsigned int>(m_Meshes.size()); }
		void SetNumMesh(unsigned int num);
		// 원래는 GetLodMesh(). 즉, lodLevel까지 받는다.
		Mesh* GetMesh(size_t batchIndex);

		const std::vector<DrawableBatch>& GetBatches() const { return m_Batches; }

	protected:
		
	protected:
		Model* m_pModel = nullptr;

		// 원래는 vector의 vector이다.
		// lod별로 관리하는 듯 하다.
		std::vector<Mesh*> m_Meshes;
		std::vector<DrawableBatch> m_Batches;
	};
}