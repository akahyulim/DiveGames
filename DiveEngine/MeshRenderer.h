#pragma once
#include "Component.h"
#include "dvMesh.h"

namespace Dive
{
	// �׸� �� ����ϴ� �����͸� �����Ѵ�.
	// ��, Buffer, Offset, Count ���� ������ �Ѵ�.
	// Renderable�̶�� �̸��� ũ�� ������ �ʴ�.
	class MeshRenderer : public Component
	{
	public:
	private:
	private:
		// �̷��� �Ѵٴ°� Spartan ���� �����ڴٴ� �Ŵ�.
		// �׸��� static, skinned�� ���� ���еǾ�� �Ѵ�.
		dvMesh* m_pMesh;
		unsigned int m_vertexOffset;
		unsigned int m_vertexCount;
		unsigned int m_indexOffset;
		unsigned int m_indexCount;
		
		// Material: �ؽ���, �� ��
		// Lighting
		
	};
}