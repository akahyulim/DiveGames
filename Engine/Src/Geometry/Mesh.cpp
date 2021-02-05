#include "DivePch.h"
#include "Mesh.h"

namespace Dive
{
	Mesh::Mesh(Context * context)
		: Resource(context),
		m_vertexBuffer(nullptr),
		m_indexBuffer(nullptr)
	{
		m_resourceType = eResourceType::Mesh;
	}

	Mesh::~Mesh()
	{
		SAFE_DELETE(m_indexBuffer);
		SAFE_DELETE(m_vertexBuffer);
	}
}