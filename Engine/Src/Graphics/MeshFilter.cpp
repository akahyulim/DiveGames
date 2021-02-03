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

		// �̰� �ȵȴ�.
		//m_mesh = std::make_unique<Dive_Mesh>(mesh);

		// vertex buffer
		{
			// �ᱹ Vertex Type���� ������ �ʿ��ϴ�.
			if (m_mesh->positions.empty())
			{
				CORE_ERROR("");
				return false;
			}

			// �� ����� ���翩�θ� �ľ��� �� |= �����ϴ°� �´�.
			// �׷��� ���۸� �����Ϸ��� ���� Ÿ���� ����ü�� �ʿ��ϴ�.
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