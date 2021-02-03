#include "DivePch.h"
#include "MeshFilter.h"


namespace Dive
{

	bool Dive_MeshFilter::SetMesh(Dive_Mesh * mesh)
	{
		if (!mesh)
		{
			CORE_ERROR("");
			return false;
		}

		// 이게 안된다.
		//m_mesh = std::make_unique<Dive_Mesh>(mesh);

		// vertex buffer
		{
			// 결국 Vertex Type으로 구성이 필요하다.
			if (m_mesh->positions.empty())
			{
				CORE_ERROR("");
				return false;
			}

			// 각 요소의 존재여부를 파악한 후 |= 연산하는게 맞다.
			// 그런데 버퍼를 생성하려면 최종 타입의 구조체가 필요하다.
		}

		// index buffer
		{

		}

		return false;
	}

	//==================================================================================================

	MeshFilter::MeshFilter(Context * context)
		: Resource(context),
		m_mesh(nullptr),
		m_vertexCount(0),
		m_indexCount(0)
	{
		m_resourceType = eResourceType::MeshFilter;
	}

	MeshFilter::~MeshFilter()
	{
		Clear();
	}

	void MeshFilter::Clear()
	{
		m_indexBuffer.reset();
		m_vertexBuffer.reset();
		SAFE_DELETE(m_mesh);

		m_indexCount	= 0;
		m_vertexCount	= 0;
	}
}