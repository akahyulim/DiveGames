#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;

	class Mesh : public Resource
	{
	public:
		// 생성자 혹은 SetMacro등의 함수로 매크로를 정의해 Input Type을 다양하게 만들 수 있다.
		// 이 말은 타입 정의의 개수만큼 셰이더도 추가로 생성된다는 의미다.
		// 셰이더 내부에서의 사용은 쉬워지지만 셰이더 자체의 관리는 어렵게 된다.
		Mesh(VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer);	// 지울까?
		Mesh(const std::string& name, VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer);
		~Mesh();

		void Draw(D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	public:
		static Mesh* Create(const std::string& name, VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer);

	private:
	private:
		VertexBuffer* m_pVertexBuffer;
		IndexBuffer* m_pIndexBuffer;
	};
}