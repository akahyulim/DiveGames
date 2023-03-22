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

		// urho�� vertex, pixel�� ��������.
		std::vector<Light*> m_Lights;

		// ��ü �޽� �������� �κ��� �׸��� geometry��.
		// vertex�� index�� offset, count�� �����Ѵ�.
		// material�� �����Ѵ�.
		// model�� resource������
		// �̸� ������ gameobject�� ��� �����ؾ� �� �� �����ؾ� �Ѵ�.
		// �׸��� ���� ���� ���¿� ���� ��ȭ�� ������ ���̴�.
		// instancing�� ����Ƽ�� ���� ���� �׸��� SkinnedMesh Ȯ�嵵 �����ؾ� �Ѵ�.
	};
}