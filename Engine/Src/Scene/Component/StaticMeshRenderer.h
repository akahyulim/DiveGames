#pragma once
#include "dvMeshRenderer.h"

namespace Dive
{
	// �̰� ���� ���� ������ �� �� ã�ƺ��� �Ѵ�.
	class StaticMeshRenderer : public dvMeshRenderer
	{
	public:
		StaticMeshRenderer(GameObject* pOwner);
		~StaticMeshRenderer();

		void Draw(ID3D11DeviceContext* pImmediateContext) override;

	private:
	private:

	};
}