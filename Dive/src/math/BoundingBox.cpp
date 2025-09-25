#include "stdafx.h"
#include "BoundingBox.h"
#include "graphics/Vertex.h"
#include "rendering/StaticMesh.h"
#include "world/components/Camera.h"

using namespace DirectX;

namespace Dive
{
	BoundingBox::BoundingBox()
	{
		m_Max = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		m_Min = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	BoundingBox::BoundingBox(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max)
		: m_Min(min)
		, m_Max(max)
	{
	}

	BoundingBox::BoundingBox(const DirectX::XMFLOAT3* points, uint32_t pointCount)
	{
		m_Max = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		m_Min = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);

		for (uint32_t i = 0; i < pointCount; i++)
		{
			m_Max.x = std::max(m_Max.x, points[i].x);
			m_Max.y = std::max(m_Max.y, points[i].y);
			m_Max.z = std::max(m_Max.z, points[i].z);

			m_Min.x = std::min(m_Min.x, points[i].x);
			m_Min.y = std::min(m_Min.y, points[i].y);
			m_Min.z = std::min(m_Min.z, points[i].z);
		}
	}

	BoundingBox::BoundingBox(const std::vector<StaticVertex>& vertices)
	{
		m_Max = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		m_Min = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);

		for (auto& vertex : vertices)
		{
			m_Max.x = std::max(m_Max.x, vertex.Position.x);
			m_Max.y = std::max(m_Max.y, vertex.Position.y);
			m_Max.z = std::max(m_Max.z, vertex.Position.z);

			m_Min.x = std::min(m_Min.x, vertex.Position.x);
			m_Min.y = std::min(m_Min.y, vertex.Position.y);
			m_Min.z = std::min(m_Min.z, vertex.Position.z);
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

	bool BoundingBox::Intersects(const Ray& ray, float* outDistance, DirectX::XMFLOAT3* outNormal) const
	{
		float tMin = 0.0f;
		float tMax = FLT_MAX;
		int hitAxis = -1;
		float sign = 0.0f;

		for (int i = 0; i < 3; ++i)
		{
			float origin = (&ray.origin.x)[i];
			float direction = (&ray.direction.x)[i];
			float minBound = (&m_Min.x)[i];
			float maxBound = (&m_Max.x)[i];

			if (fabs(direction) < 1e-6f)
			{
				if (origin < minBound || origin > maxBound)
					return false;
			}
			else
			{
				float invDir = 1.0f / direction;
				float t1 = (minBound - origin) * invDir;
				float t2 = (maxBound - origin) * invDir;

				if (t1 > t2) std::swap(t1, t2);

				if (t1 > tMin)
				{
					tMin = t1;
					hitAxis = i;
					sign = (direction > 0.0f) ? -1.0f : 1.0f;
				}

				tMax = std::min(tMax, t2);
				if (tMin > tMax)
					return false;
			}
		}

		if (outDistance) *outDistance = tMin;

		if (outNormal)
		{
			*outNormal = { 0, 0, 0 };
			(&outNormal->x)[hitAxis] = sign;
		}

		return true;
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
		XMFLOAT3 corners[8] =
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
			auto point = XMLoadFloat3(&corners[i]);
			point = XMVector3Transform(point, transform);
			XMStoreFloat3(&corners[i], point);
		}

		XMFLOAT3 minPoint = corners[0];
		XMFLOAT3 maxPoint = corners[0];

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
