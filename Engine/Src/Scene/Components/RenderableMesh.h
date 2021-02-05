#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;
	class Mesh;
	class Material;

	class RenderableMesh : public Component
	{
		DIVE_OBJECT(RenderableMesh, Component);

	public:
		RenderableMesh(Context* context, GameObject* owner);
		virtual ~RenderableMesh();

		// components

		// mesh
		Mesh* GetMesh() const		{ return m_mesh; }
		virtual bool SetMesh(Mesh* mesh);

		// materials
		unsigned int GetMaterialCount() const { return static_cast<unsigned int>(m_materials.size()); }
		Material* GetMaterial(size_t index) const;
		const std::vector<Material*>& GetAllMaterial() const { return m_materials; }
		bool AddMaterial(Material* material);

		// light & shadow

	protected:

	protected:
		Mesh* m_mesh;
		std::vector<Material*> m_materials;
	};
}