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
		// 아래와 같은 이유로 구분되어야 할 것 같다.
		virtual bool SetMesh(Mesh* mesh);

		// materials
		Material* GetMaterial() { return m_material; }
		// get은 파일로부터 생성하거나, 생성된 것을 가져와야 한다.
		void SetMaterial(const std::string& filepath);
		// 생성된 것이라 함은 테스트에서 구현한 것과 같이 코드 상에서 직접 만든 것이다.
		// 따라서 이를 Cache에 넣으면서 파일화하는 작업이 필요하다.


		// light & shadow

	protected:

	protected:
		Mesh* m_mesh;
		Material* m_material;
	};
}