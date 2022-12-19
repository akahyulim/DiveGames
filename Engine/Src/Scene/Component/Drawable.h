#pragma once
#include "Component.h"
#include "Graphics/GraphicsDefs.h"

namespace Dive
{
	class Context;
	class Mesh;
	class Model;
	class Material;
	
	struct DrawableSourceData
	{
		DrawableSourceData() = default;
		DrawableSourceData(const DrawableSourceData& rhs) = default;
		~DrawableSourceData() = default;

		DrawableSourceData& operator=(const DrawableSourceData& rhs) = default;

		float Distance;
		Mesh* pMesh;
		Material* pMaterial;
		const DirectX::XMFLOAT4X4* pWorldTransform;
		eGeometryType GeometryType;
	};

	class Drawable : public Component
	{
		DIVE_OBJECT(Drawable, Component)

	public:
		explicit Drawable(Context* pContext);
		~Drawable() override;

		virtual void Update();

		Model* GetModel() const { return m_pModel; }
		virtual void SetModel(Model* pModel);

		virtual Material* GetMaterial() const { return GetMaterial(0); }
		virtual void SetMaterial(Material* material);
		virtual Material* GetMaterial(size_t index) const;
		virtual bool SetMaterial(size_t index, Material* material);

		uint32_t GetMeshCount() const { return static_cast<uint32_t>(m_Meshes.size()); }
		void SetNumMesh(uint32_t num);

		Mesh* GetMesh(size_t batchIndex);

		const std::vector<DrawableSourceData>& GetSourceDatas() const { return m_SourceDatas; }

	protected:
		
	protected:
		Model* m_pModel;

		std::vector<Mesh*> m_Meshes;
		std::vector<DrawableSourceData> m_SourceDatas;
	};
}