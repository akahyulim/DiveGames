#pragma once

namespace Dive
{
	struct VertexStatic;

	enum class eIntersection
	{
		Intersect,
		Inside,
		Outsize
	};

	class BoundingBox
	{
	public:
		BoundingBox();
		BoundingBox(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max);
		BoundingBox(const DirectX::XMFLOAT3* points, UINT32 pointCount);
		BoundingBox(const VertexStatic* vertices, UINT32 vertexCount);
		~BoundingBox() = default;

		BoundingBox& operator=(const BoundingBox&) = default;

		eIntersection IsIntersect(const DirectX::XMFLOAT3& point) const;
		eIntersection IsIntersect(const BoundingBox& box) const;

		void Merge(const BoundingBox& box);

		BoundingBox Transform(const DirectX::XMMATRIX& transform) const;

		DirectX::XMFLOAT3 GetCenter() const;
		DirectX::XMFLOAT3 GetSize() const;
		DirectX::XMFLOAT3 GetExtent() const;

		DirectX::XMFLOAT3 GetMin() const { return m_Min; }
		DirectX::XMFLOAT3 GetMax() const { return m_Max; }
	
	private:
		DirectX::XMFLOAT3 m_Min;
		DirectX::XMFLOAT3 m_Max;
	};
}