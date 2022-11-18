#pragma once
#include "Component.h"

namespace Dive
{
	class Context;
	class Mesh;
	class Model;
	class Material;

	// ���� GraphicsDef�� �̵�
	enum class eMeshType
	{
		Static,
		Skinned,
		Instanced,
		Billboard,
	};
	
	// �ϳ��� ���� �ټ��� �޽ÿ� ��Ƽ����� ������ �� �ֱ⿡
	// �̵��� ������ ���� ������ �ʿ䰡 �ִ�.
	struct DrawableBatch
	{
		DrawableBatch() = default;
		DrawableBatch(const DrawableBatch& rhs) = default;
		~DrawableBatch() = default;

		DrawableBatch& operator=(const DrawableBatch& rhs) = default;

		float m_Distance = 0.0f;
		Mesh* m_pMesh = nullptr;
		Material* m_pMaterial = nullptr;
		DirectX::XMFLOAT4X4 m_WorldTransform;	// skinned ���̶�� �Ѵ�...
		eMeshType m_MeshType;
	};

	class Drawable : public Component
	{
		DIVE_OBJECT(Drawable, Component)

	public:
		explicit Drawable(Context* pContext);
		~Drawable() override;

		// virtual update => StaticDrawble���� �̱����̴�. �Դٰ� worker thread�� ȣ���Ѵٰ�...
		// virtual updateBatches
		// virtual updateMeshes => StaticDrawble���� �̱����̴�.

		Model* GetModel() const { return m_pModel; }
		virtual void SetModel(Model* pModel);

		virtual Material* GetMaterial() const { return GetMaterial(0); }
		virtual void SetMaterial(Material* pMaterial);
		virtual Material* GetMaterial(size_t index) const;
		virtual bool SetMaterial(size_t index, Material* pMaterial);

		unsigned int GetNumMesh() const { return static_cast<unsigned int>(m_Meshes.size()); }
		void SetNumMesh(unsigned int num);
		// ������ GetLodMesh(). ��, lodLevel���� �޴´�.
		Mesh* GetMesh(size_t batchIndex);

		const std::vector<DrawableBatch>& GetBatches() const { return m_Batches; }

	protected:
		
	protected:
		Model* m_pModel = nullptr;

		// ������ vector�� vector�̴�.
		// lod���� �����ϴ� �� �ϴ�.
		std::vector<Mesh*> m_Meshes;
		std::vector<DrawableBatch> m_Batches;
	};
}