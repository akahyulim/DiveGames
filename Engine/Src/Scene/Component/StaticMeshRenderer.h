#pragma once
#include "dvMeshRenderer.h"

namespace DiveEngine
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