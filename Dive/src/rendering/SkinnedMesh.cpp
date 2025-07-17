#include "stdafx.h"
#include "skinnedMesh.h"
#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"

namespace Dive
{
	SkinnedMesh::SkinnedMesh()
	{
		m_Type = eMeshType::Skinned;
	}

	bool SkinnedMesh::CreateBuffers()
	{
		

		return true;
	}

	void SkinnedMesh::Clear()
	{
		
	}
}
