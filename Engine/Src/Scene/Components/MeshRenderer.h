#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;
	class Mesh;
	class StaticMesh;
	class Material;

	// 이름이 좀 아쉽다.
	// 그리고 유니티의 경우 SkinnedMeshRenderer는 MeshRenderer를 상속한다.
	// => 이는 StaticMesh와 SkinnnedMesh가 구분되어 있지 않기 때문인 듯 하다.
	// => 즉, VertexBuffer를 구분없이 생성할 수 있다면 나 역시 Mesh를 한 가지 타입으로 관리할 수 있다.
	// => 이를 위해선 버퍼를 미리 생성해놓고 맵/언맵을 통해 데이터를 전달하면 된다.
	// => 다만 이 경우 인풋레이아웃도 별도로 구성을 지원해야 한다.
	class IMeshRenderer : public Component
	{
	public:
		IMeshRenderer(GameObject* pGameObject);
		virtual ~IMeshRenderer();

		virtual void Draw() = 0;

		virtual Mesh* GetMesh() = 0;
		virtual void SetMesh(Mesh* pMesh) = 0;

		bool HasMaterial() const { return m_pMaterial; }
		Material* GetMaterial() const { return m_pMaterial; }
		void SetMaterial(Material* pMaterial) { m_pMaterial = pMaterial; }

	protected:
		Material* m_pMaterial;
	};

	class MeshRenderer : public IMeshRenderer
	{
	public:
		MeshRenderer(GameObject* pGameObject);
		virtual ~MeshRenderer();

		void Draw() override;

		Mesh* GetMesh() override;
		void SetMesh(Mesh* pMesh) override;

	private:

	private:
		StaticMesh* m_pMesh;
	};
}