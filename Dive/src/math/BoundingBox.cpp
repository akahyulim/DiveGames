#include "stdafx.h"
#include "BoundingBox.h"
#include "core/CoreDefs.h"
#include "rendering/Mesh.h"

namespace Dive
{
	BoundingBox::BoundingBox()
	{
		m_Max = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		m_Min = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	BoundingBox::BoundingBox(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max)
		: m_Min(min)
		, m_Max(max)
	{
	}

	BoundingBox::BoundingBox(const DirectX::XMFLOAT3* points, UINT32 pointCount)
	{
		m_Max = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		m_Min = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);

		for (UINT32 i = 0; i < pointCount; i++)
		{
			m_Max.x = std::max(m_Max.x, points[i].x);
			m_Max.y = std::max(m_Max.y, points[i].y);
			m_Max.z = std::max(m_Max.z, points[i].z);

			m_Min.x = std::min(m_Min.x, points[i].x);
			m_Min.y = std::min(m_Min.y, points[i].y);
			m_Min.z = std::min(m_Min.z, points[i].z);
		}
	}

	BoundingBox::BoundingBox(const VertexStatic* vertices, UINT32 vertexCount)
	{
		m_Max = DirectX::XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		m_Min = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);

		for (UINT32 i = 0; i < vertexCount; i++)
		{
			m_Max.x = std::max(m_Max.x, vertices[i].position[0]);
			m_Max.y = std::max(m_Max.y, vertices[i].position[1]);
			m_Max.z = std::max(m_Max.z, vertices[i].position[2]);

			m_Min.x = std::min(m_Min.x, vertices[i].position[0]);
			m_Min.y = std::min(m_Min.y, vertices[i].position[1]);
			m_Min.z = std::min(m_Min.z, vertices[i].position[2]);
		}
	}

	eIntersection BoundingBox::IsIntersect(const DirectX::XMFLOAT3& point) const
	{
		if(point.x < m_Min.x || point.y < m_Min.y || point.z < m_Min.z ||
			point.x > m_Max.x || point.y > m_Max.y || point.z > m_Max.z)
			return eIntersection::Outsize;

		return eIntersection::Inside;
	}

	eIntersection BoundingBox::IsIntersect(const BoundingBox& box) const
	{
		if (box.m_Max.x < m_Min.x || box.m_Max.y < m_Min.y || box.m_Max.z < m_Min.z ||
			box.m_Min.x > m_Max.x || box.m_Min.y > m_Max.y || box.m_Min.z > m_Max.z)
			return eIntersection::Outsize;
		else if (box.m_Min.x < m_Min.x || box.m_Min.y < m_Min.y || box.m_Min.z < m_Min.z ||
			box.m_Max.x > m_Max.x || box.m_Max.y > m_Max.y || box.m_Max.z > m_Max.z)
			return eIntersection::Intersect;
		else
			return eIntersection::Inside;
	}

	void BoundingBox::Merge(const BoundingBox& box)
	{
		m_Min.x = std::min(m_Min.x, box.m_Min.x);
		m_Min.y = std::min(m_Min.y, box.m_Min.y);
		m_Min.z = std::min(m_Min.z, box.m_Min.z);

		m_Max.x = std::max(m_Max.x, box.m_Max.x);
		m_Max.y = std::max(m_Max.y, box.m_Max.y);
		m_Max.z = std::max(m_Max.z, box.m_Max.z);
	}

	BoundingBox BoundingBox::Transform(const DirectX::XMMATRIX& transform) const
	{
		DirectX::XMFLOAT3 corners[8] =
		{
			{ m_Min.x, m_Min.y, m_Min.z },
			{ m_Min.x, m_Min.y, m_Max.z },
			{ m_Min.x, m_Max.y, m_Min.z },
			{ m_Min.x, m_Max.y, m_Max.z },
			{ m_Max.x, m_Min.y, m_Min.z },
			{ m_Max.x, m_Min.y, m_Max.z },
			{ m_Max.x, m_Max.y, m_Min.z },
			{ m_Max.x, m_Max.y, m_Max.z }
		};

		for (int i = 0; i < 8; ++i)
		{
			DirectX::XMVECTOR point = DirectX::XMLoadFloat3(&corners[i]);
			point = DirectX::XMVector3Transform(point, transform);
			DirectX::XMStoreFloat3(&corners[i], point);
		}

		DirectX::XMFLOAT3 minPoint = corners[0];
		DirectX::XMFLOAT3 maxPoint = corners[0];

		for (int i = 1; i < 8; ++i)
		{
			minPoint.x = std::min(minPoint.x, corners[i].x);
			minPoint.y = std::min(minPoint.y, corners[i].y);
			minPoint.z = std::min(minPoint.z, corners[i].z);
			maxPoint.x = std::max(maxPoint.x, corners[i].x);
			maxPoint.y = std::max(maxPoint.y, corners[i].y);
			maxPoint.z = std::max(maxPoint.z, corners[i].z);
		}

		return BoundingBox(minPoint, maxPoint);
	}
	
	DirectX::XMFLOAT3 BoundingBox::GetCenter() const
	{
		return {
			(m_Max.x + m_Min.x) * 0.5f,
			(m_Max.y + m_Min.y) * 0.5f,
			(m_Max.z + m_Min.z) * 0.5f
		};
	}

	DirectX::XMFLOAT3 BoundingBox::GetSize() const
	{
		return {
			(m_Max.x - m_Min.x),
			(m_Max.y - m_Min.y),
			(m_Max.z - m_Min.z)
		};
	}
	
	DirectX::XMFLOAT3 BoundingBox::GetExtent() const
	{
		return {
			(m_Max.x - m_Min.x) * 0.5f,
			(m_Max.y - m_Min.y) * 0.5f,
			(m_Max.z - m_Min.z) * 0.5f
		};
	}
}