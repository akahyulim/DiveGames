#pragma once
#include "Component.h"
#include "Mesh.h"

namespace dive
{
	enum class eDefaultMeshType
	{
		Triangle,
		Quad,
		Cube,
		Sphere,
		Capsule,
	};

	// static mesh��.
	class MeshRenderer : public Component
	{
	public:
		MeshRenderer();
		~MeshRenderer();

		// Virtual
		void Serialize(FileStream* fileStream) override;
		void Deserialize(FileStream* fileStream) override;

		void CreateDefaultMesh(eDefaultMeshType type);


		// �ƿ� �� ���� Draw �Լ��� ������� �ִ�.
		// ������ IL�� Shader�� ���� ������ �� �־�� �Ѵ�.
		ID3D11Buffer* GetVertexBuffer() { return mVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return mIndexBuffer; }

		unsigned int GetVertexStride() { return static_cast<unsigned int>(sizeof(VertexType_PosTexNorTan)); }
		unsigned int GetIndexCount() { return mMesh->GetIndexCount(); }

	private:
		void calcu();
		bool createBuffers();

	private:
		Mesh* mMesh = nullptr;
		// Material: �ؽ���, �� ��
		// Lighting
		
		// ���� �� ������ Draw�� �����Ѵٸ�
		// �Ʒ� ���۴� unique_ptr�� ��︰��.
		ID3D11Buffer* mVertexBuffer = nullptr;
		ID3D11Buffer* mIndexBuffer = nullptr;

		unsigned int mStride = 0;
		// �ϴ� offset�� 0����...

		// ������ ���� �� �ִ�.
		// �����ٸ� �ش� ������ GameObejct(Transform)�� vertices, indices�� offset �� count�� �����ؾ� �Ѵ�.
		// �׷��� �׸� �� ���� �� �ִ�.
		// �׷��� ����Ƽ�� MeshRenderer�� Transform�� �翬�� �ִ�. �̰� �ǹ̰� ���°ǰ�?
	};
}