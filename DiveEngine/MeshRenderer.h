#pragma once
#include "Component.h"
#include "dvMesh.h"

namespace dive
{
	// �ϴ� �̰� base�� �Ѵ�.
	// ���� 2D, Static, Skinned�� ���� �� �ִ�.
	// �������� �� �� ���߿� ��������.
	class MeshRenderer : public Component
	{
	public:
	private:
	private:
		// Mesh : ���ҽ���.
		// Material: �ؽ���, �� ��
		// Lighting
		
		// vertex buffer
		// index buffer

		// ������ ���� �� �ִ�.
		// �����ٸ� �ش� ������ GameObejct(Transform)�� vertices, indices�� offset �� count�� �����ؾ� �Ѵ�.
		// �׷��� �׸� �� ���� �� �ִ�.
		// �׷��� ����Ƽ�� MeshRenderer�� Transform�� �翬�� �ִ�. �̰� �ǹ̰� ���°ǰ�?
	};
}