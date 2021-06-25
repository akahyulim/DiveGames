#pragma once
#include "Component.h"
#include "dvMesh.h"

namespace dive
{
	// static mesh다.
	class MeshRenderer : public Component
	{
	public:
		MeshRenderer();
		~MeshRenderer();

		// 추후 바꿔야할 거다.
		void SetMesh(const dvMesh& mesh);

		// 아에 이 곳에 Draw 함수를 만들수도 있다.
		// 하지만 IL과 Shader를 구분 선택할 수 있어야 한다.
		ID3D11Buffer* GetVertexBuffer() { return mVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return mIndexBuffer; }

		unsigned int GetVertexStride() { return static_cast<unsigned int>(sizeof(VertexType_PosTexNorTan)); }
		unsigned int GetIndexCount() { return mMesh->GetIndexCount(); }

	private:
		void calcu();
		bool createBuffers();

	private:
		dvMesh* mMesh = nullptr;
		// Material: 텍스쳐, 색 등
		// Lighting
		
		// 만약 이 곳에서 Draw를 구현한다면
		// 아래 버퍼는 unique_ptr이 어울린다.
		ID3D11Buffer* mVertexBuffer = nullptr;
		ID3D11Buffer* mIndexBuffer = nullptr;

		unsigned int mStride = 0;
		// 일단 offset은 0으로...

		// 부위가 나뉠 수 있다.
		// 나뉜다면 해당 부위별 GameObejct(Transform)과 vertices, indices의 offset 및 count를 관리해야 한다.
		// 그래야 그릴 때 나눌 수 있다.
		// 그런데 유니티엔 MeshRenderer도 Transform이 당연히 있다. 이건 의미가 없는건가?
	};
}