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


		// 아니면 아에 이 곳에서 Draw()를 구성하는 것도 한 방법이다.
		// 물론 그렇다면 SkinnedMesh일 때 좀 더 생각할 구석이 많아진다.
		// 스파르탄 형식은 안되기 때문이다?
		// 일단 이걸 그릴려면 IL과 이에 맞는 Shader를 또 만들어야 한다.
		ID3D11Buffer* GetVertexBuffer() { return mVertexBuffer; }
		ID3D11Buffer* GetIdexBuffer() { return mIndexBuffer; }

		unsigned int GetVertexStride() { return mStride; }

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