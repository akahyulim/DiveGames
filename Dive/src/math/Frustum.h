#pragma once

namespace Dive
{
	class Frustum
	{
	public:
		Frustum() = default;
		~Frustum() = default;

		void Update(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& proj);

		bool CheckAABB(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents) const;
		bool CheckSphere(const DirectX::XMFLOAT3& center, float radius) const;

	private:
		DirectX::XMVECTOR m_Planes[6];
	};
}
