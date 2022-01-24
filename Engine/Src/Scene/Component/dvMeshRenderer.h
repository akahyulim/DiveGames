#pragma once
#include "Component.h"
#include "../Src/Renderer/Graphics/GraphicsDevice.h"

namespace Dive
{
	class GameObject;
	class dvMesh;

	class dvMeshRenderer : public Component
	{
	public:
		dvMeshRenderer(GameObject* pGameObject);
		~dvMeshRenderer();

		// Virtual
	//	void Serialize(FileStream* pFileStream) override;
	//	void Deserialize(FileStream* pFileStream) override;

		dvMesh* GetMesh() { return m_pMesh; }		// const�� ���� �� ����. �׷��� Mesh�� Get �Լ��鵵 const���� �Ѵ�.
		void SetMesh(dvMesh* pMesh) { m_pMesh = pMesh; }

		virtual void Draw(ID3D11DeviceContext* pImmediateContext) = 0;

	protected:
		dvMesh* m_pMesh;

	private:
	private:
	};
}