#pragma once
#include "Component.h"

namespace Dive
{
	class Context;
	class Model;
	class Mesh;

	class StaticModel : public Component
	{
		DIVE_OBJECT(StaticModel, Component)

	public:
		explicit StaticModel(Context* pContext);
		~StaticModel() override;

		Model* GetModel() { return m_pModel; }
		virtual void SetModel(Model* pModel);

		unsigned int GetNumMeshes() const { return (unsigned int)(m_Meshes.size()); }
		void SetNumMeshes(unsigned int num);

		// ���� �̸��� GetLodGeometry, batchIndex�� lodLevel�� �޴´�.
		Mesh* GetMesh(unsigned int index);

	private:
	private:
		Model* m_pModel;
		// ������ vector�� vector��. geometry�� lodLevel���� �����ϱ� �����ε�...?
		std::vector<Mesh*> m_Meshes;
	};
}