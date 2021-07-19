#pragma once
#include "Component.h"
#include "Mesh.h"

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
		MeshRenderer(GameObject* gameObject);
		~MeshRenderer();

		// Virtual
		void Serialize(FileStream* fileStream) override;
		void Deserialize(FileStream* fileStream) override;

		void CreateDefaultMesh(eDefaultMeshType type);
		void LoadMeshFromFile(const std::string& filepath);


		// �ƿ� �� ���� Draw �Լ��� ������� �ִ�.
		// ������ IL�� Shader�� ���� ������ �� �־�� �Ѵ�.
		ID3D11Buffer* GetVertexBuffer() { return m_VertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_IndexBuffer; }

		unsigned int GetVertexStride() { return static_cast<unsigned int>(sizeof(VertexType_PosTexNorTan)); }
		unsigned int GetIndexCount() { return m_Mesh->GetIndexCount(); }

		// �̰� ��°�� �Ѱ���� �ϳ�...? �׷� unique_ptr�� ������
		Mesh* GetMesh() { return m_Mesh; }

		// test
		void Render(ID3D11DeviceContext* deviceContext);

	private:
		void calcu();
		bool createBuffers();

	private:
		Mesh* m_Mesh = nullptr;
		// Material: �ؽ���, �� ��
		// Lighting
		
		// ���� �� ������ Draw�� �����Ѵٸ�
		// �Ʒ� ���۴� unique_ptr�� ��︰��.
		ID3D11Buffer* m_VertexBuffer = nullptr;
		ID3D11Buffer* m_IndexBuffer = nullptr;

		unsigned int m_Stride = 0;
		// �ϴ� offset�� 0����...

		eDefaultMeshType m_MeshType = eDefaultMeshType::None;

		// ������ ���� �� �ִ�.
		// �����ٸ� �ش� ������ GameObejct(Transform)�� vertices, indices�� offset �� count�� �����ؾ� �Ѵ�.
		// �׷��� �׸� �� ���� �� �ִ�.
		// �׷��� ����Ƽ�� MeshRenderer�� Transform�� �翬�� �ִ�. �̰� �ǹ̰� ���°ǰ�?
	};
}