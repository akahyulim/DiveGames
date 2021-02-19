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
		// �Ʒ��� ���� ������ ���еǾ�� �� �� ����.
		virtual bool SetMesh(Mesh* mesh);

		// materials
		Material* GetMaterial() { return m_material; }
		// get�� ���Ϸκ��� �����ϰų�, ������ ���� �����;� �Ѵ�.
		void SetMaterial(const std::string& filepath);
		// ������ ���̶� ���� �׽�Ʈ���� ������ �Ͱ� ���� �ڵ� �󿡼� ���� ���� ���̴�.
		// ���� �̸� Cache�� �����鼭 ����ȭ�ϴ� �۾��� �ʿ��ϴ�.


		// light & shadow

	protected:

	protected:
		Mesh* m_mesh;
		Material* m_material;
	};
}