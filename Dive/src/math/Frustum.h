#pragma once
#include "BoundingBox.h"

namespace Dive
{
	class Frustum
	{
	public:
		Frustum() = default;
		~Frustum() = default;

		void Update(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);

		bool CheckCenter(const DirectX::XMFLOAT3& center) const;
		bool CheckAABB(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents) const;
		bool CheckSphere(const DirectX::XMFLOAT3& center, float radius) const;

	private:
		DirectX::XMVECTOR m_Planes[6];
	};
}
