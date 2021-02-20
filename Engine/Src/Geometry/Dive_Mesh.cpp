#include "DivePch.h"
#include "Dive_Mesh.h"
#include "Graphics/D3D11/VertexBuffer.h"
#include "Graphics/D3D11/IndexBuffer.h"

namespace Dive
{
	Dive_MeshFilter::Dive_MeshFilter(Context * context)
		: Resource(context)
	{
	}

	Dive_MeshFilter::~Dive_MeshFilter()
	{
	}
	
	void Dive_MeshFilter::SetMesh(const Dive_Mesh & mesh)
	{
	}
	
	bool Dive_MeshFilter::CreateBuffers()
	{
		// mesh의 type을 알고 있으므로 type 별 Vertex Buffer 생성 가능
		// 그런데 Vertex Data를 하나의 배열 혹은 vector로 구성해야 한다.

		return false;
	}

	void Dive_MeshFilter::indentifyMeshType()
	{
		// mesh의 요소 data 유무를 통해 vertex type 확인
		// | 연산을 통해 추가하고 싶은데...
		/*
		if (m_mesh.vertices.empty())
		{

		}
		if (!m_mesh.colors.empty())
		{

		}
		if (!m_mesh.texCoords.empty())
		{

		}
		if (!m_mesh.normals.empty())
		{

		}
		*/
		// 타입 저장

		// 버퍼를 만들려면 Vertex 정보들을 Vector로 다시 구성해야 한다.
		// 그런데 이 구성 자체가 VertexType을 명시적으로 설정해야만 한다.
		// 즉, 미리 각각의 타입별 vector가 미리 준비되어 있어야 한다는 이야기이다.
	}
}