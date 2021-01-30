#include "DivePch.h"
#include "MeshFilter.h"


namespace Dive
{
	MeshFilter::MeshFilter(Context * context)
		: Resource(context, eResourceType::MeshFilter),
		m_mesh(nullptr),
		m_vertexCount(0),
		m_indexCount(0)
	{
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