#pragma once
#include "dvMeshRenderer.h"

namespace Dive
{
	// 이건 아직 구분 사항을 좀 더 찾아봐야 한다.
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