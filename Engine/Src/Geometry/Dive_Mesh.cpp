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
		// mesh�� type�� �˰� �����Ƿ� type �� Vertex Buffer ���� ����
		// �׷��� Vertex Data�� �ϳ��� �迭 Ȥ�� vector�� �����ؾ� �Ѵ�.

		return false;
	}

	void Dive_MeshFilter::indentifyMeshType()
	{
		// mesh�� ��� data ������ ���� vertex type Ȯ��
		// | ������ ���� �߰��ϰ� ������...
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
		// Ÿ�� ����

		// ���۸� ������� Vertex �������� Vector�� �ٽ� �����ؾ� �Ѵ�.
		// �׷��� �� ���� ��ü�� VertexType�� ��������� �����ؾ߸� �Ѵ�.
		// ��, �̸� ������ Ÿ�Ժ� vector�� �̸� �غ�Ǿ� �־�� �Ѵٴ� �̾߱��̴�.
	}
}