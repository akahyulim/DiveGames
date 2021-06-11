#pragma once
#include "Component.h"
#include "dvMesh.h"

namespace Dive
{
	// 그릴 때 사용하는 데이터를 전달한다.
	// 즉, Buffer, Offset, Count 등을 가져야 한다.
	// Renderable이라는 이름도 크게 나쁘진 않다.
	class MeshRenderer : public Component
	{
	public:
	private:
	private:
		// 이렇게 한다는건 Spartan 모델을 따르겠다는 거다.
		// 그리고 static, skinned에 따라 구분되어야 한다.
		dvMesh* m_pMesh;
		unsigned int m_vertexOffset;
		unsigned int m_vertexCount;
		unsigned int m_indexOffset;
		unsigned int m_indexCount;
		
		// Material: 텍스쳐, 색 등
		// Lighting
		
	};
}