#pragma once
#include "Component.h"
#include "../../Renderer/Mesh.h"

namespace dive
{
	class GameObject;

	enum class eDefaultMeshType
	{
		Triangle,
		Quad,
		Cube,
		Sphere,
		Capsule,
		None
	};

	// static mesh��.
	class MeshRenderer : public Component
	{
	public:
		MeshRenderer(GameObject* pGameObject);
		~MeshRenderer();

		// Virtual
		void Serialize(FileStream* pFileStream) override;
		void Deserialize(FileStream* pFileStream) override;

		void CreateDefaultMesh(eDefaultMeshType type);
		void LoadMeshFromFile(const std::string& filepath);


		// �ƿ� �� ���� Draw �Լ��� ������� �ִ�.
		// ������ IL�� Shader�� ���� ������ �� �־�� �Ѵ�.
		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }

		unsigned int GetVertexStride() { return static_cast<unsigned int>(sizeof(VertexType_PosTexNorTan)); }
		unsigned int GetIndexCount() { return m_pMesh->GetIndexCount(); }

		bool IsReceiveShadows() const { return m_bReceiveShadows; }
		void SetReceiveShadows(bool receive) { m_bReceiveShadows = receive; }

		// �̰� ��°�� �Ѱ���� �ϳ�...? �׷� unique_ptr�� ������
		Mesh* GetMesh() { return m_pMesh; }

		// test
		void Render(ID3D11DeviceContext* deviceContext);

	private:
		void calcu();
		bool createBuffers();

	private:
		Mesh* m_pMesh = nullptr;
		// Material: �ؽ���, �� ��
		// Lighting
		
		// ���� �� ������ Draw�� �����Ѵٸ�
		// �Ʒ� ���۴� unique_ptr�� ��︰��.
		ID3D11Buffer* m_pVertexBuffer = nullptr;
		ID3D11Buffer* m_pIndexBuffer = nullptr;

		unsigned int m_Stride = 0;
		// �ϴ� offset�� 0����...

		eDefaultMeshType m_MeshType = eDefaultMeshType::None;

		bool m_bReceiveShadows = false;

		// ������ ���� �� �ִ�.
		// �����ٸ� �ش� ������ GameObejct(Transform)�� vertices, indices�� offset �� count�� �����ؾ� �Ѵ�.
		// �׷��� �׸� �� ���� �� �ִ�.
		// �׷��� ����Ƽ�� MeshRenderer�� Transform�� �翬�� �ִ�. �̰� �ǹ̰� ���°ǰ�?
	};
}