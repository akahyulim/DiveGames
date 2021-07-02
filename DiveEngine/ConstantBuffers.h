#pragma once
#include "GraphicsInclude.h"

// �̸��� �ٲ�� �� �� ����.
namespace dive
{
	// �ϴ� ��������
	// ���� �� �߰�
	struct BufferFrame
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projPerspective;
		DirectX::XMFLOAT4X4 projOrtho;
		DirectX::XMFLOAT4X4 viewProj;

		// �ϴ� �� �Լ��� ����.
	};

	struct BufferObject
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 wvp;
	};
}