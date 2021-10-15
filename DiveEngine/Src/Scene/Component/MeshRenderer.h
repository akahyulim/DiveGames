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

	// static mesh다.
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


		// 아에 이 곳에 Draw 함수를 만들수도 있다.
		// 하지만 IL과 Shader를 구분 선택할 수 있어야 한다.
		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }

		unsigned int GetVertexStride() { return static_cast<unsigned int>(sizeof(VertexType_PosTexNorTan)); }
		unsigned int GetIndexCount() { return m_pMesh->GetIndexCount(); }

		bool IsReceiveShadows() const { return m_bReceiveShadows; }
		void SetReceiveShadows(bool receive) { m_bReceiveShadows = receive; }

		// 이걸 통째로 넘겨줘야 하나...? 그럼 unique_ptr로 못만듬
		Mesh* GetMesh() { return m_pMesh; }

		// test
		void Render(ID3D11DeviceContext* deviceContext);

	private:
		void calcu();
		bool createBuffers();

	private:
		Mesh* m_pMesh = nullptr;
		// Material: 텍스쳐, 색 등
		// Lighting
		
		// 만약 이 곳에서 Draw를 구현한다면
		// 아래 버퍼는 unique_ptr이 어울린다.
		ID3D11Buffer* m_pVertexBuffer = nullptr;
		ID3D11Buffer* m_pIndexBuffer = nullptr;

		unsigned int m_Stride = 0;
		// 일단 offset은 0으로...

		eDefaultMeshType m_MeshType = eDefaultMeshType::None;

		bool m_bReceiveShadows = false;

		// 부위가 나뉠 수 있다.
		// 나뉜다면 해당 부위별 GameObejct(Transform)과 vertices, indices의 offset 및 count를 관리해야 한다.
		// 그래야 그릴 때 나눌 수 있다.
		// 그런데 유니티엔 MeshRenderer도 Transform이 당연히 있다. 이건 의미가 없는건가?
	};
}