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

		// 원래 이름은 GetLodGeometry, batchIndex와 lodLevel을 받는다.
		Mesh* GetMesh(unsigned int index);

	private:
	private:
		Model* m_pModel;
		// 원래는 vector의 vector다. geometry를 lodLevel별로 구분하기 위함인듯...?
		std::vector<Mesh*> m_Meshes;
	};
}