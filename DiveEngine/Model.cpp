#include "Model.h"
#include "dvMesh.h"
#include "Renderer.h"
#include "Log.h"

namespace dive
{
	Model::Model()
	{
		m_pMesh = nullptr;
	}

	Model::~Model()
	{
	}
	
	void Model::Clear()
	{
		// ���۸� �����Ѵ�.
	}
	
	void Model::SetMesh(dvMesh* pMesh)
	{
		// �ϴ� ���� �޽ÿ� ��
		// ���ٸ� ����
		// �ٸ��ٸ� clear �� ����
		// �׸��� ���� ����
	}

	bool Model::createBuffers()
	{
		// ��Һ��� ����� ���۸� �����Ѵ�.
		// ��� ��Ҵ� ���� ��� ���� ����Ͽ� �����Ѵ�.

		return false;
	}
}