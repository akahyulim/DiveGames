#pragma once
#include "Component.h"
#include "dvMesh.h"

namespace dive
{
	// 일단 이건 base로 한다.
	// 추후 2D, Static, Skinned로 나뉠 수 있다.
	// 프리팹은 좀 더 나중에 생각하자.
	class MeshRenderer : public Component
	{
	public:
	private:
	private:
		// Mesh : 리소스다.
		// Material: 텍스쳐, 색 등
		// Lighting
		
		// vertex buffer
		// index buffer

		// 부위가 나뉠 수 있다.
		// 나뉜다면 해당 부위별 GameObejct(Transform)과 vertices, indices의 offset 및 count를 관리해야 한다.
		// 그래야 그릴 때 나눌 수 있다.
		// 그런데 유니티엔 MeshRenderer도 Transform이 당연히 있다. 이건 의미가 없는건가?
	};
}