#include "stdafx.h"
#include "Frustum.h"

using namespace DirectX;

namespace Dive 
{
    void Frustum::Update(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
    {
        XMMATRIX viewProj = XMMatrixMultiply(view, proj);
        XMMATRIX m = XMMatrixTranspose(viewProj);

        m_Planes[0] = XMPlaneNormalize(m.r[3] + m.r[0]); // Left
        m_Planes[1] = XMPlaneNormalize(m.r[3] - m.r[0]); // Right
        m_Planes[2] = XMPlaneNormalize(m.r[3] - m.r[1]); // Top
        m_Planes[3] = XMPlaneNormalize(m.r[3] + m.r[1]); // Bottom
        m_Planes[4] = XMPlaneNormalize(m.r[2]);          // Near
        m_Planes[5] = XMPlaneNormalize(m.r[3] - m.r[2]); // Far
    }

    bool Frustum::CheckCenter(const XMFLOAT3& center) const
    {
        for (int i = 0; i < 6; ++i)
        {
            const XMVECTOR plane = m_Planes[i];
            float distance = center.x * plane.m128_f32[0] +
                center.y * plane.m128_f32[1] +
                center.z * plane.m128_f32[2] +
                plane.m128_f32[3];

            if (distance < 0.0f)
                return false;
        }
        return true;
    }

    bool Frustum::CheckAABB(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents) const
    {
        DirectX::XMFLOAT3 corners[8] = {
            { center.x - extents.x, center.y - extents.y, center.z - extents.z },
            { center.x + extents.x, center.y - extents.y, center.z - extents.z },
            { center.x - extents.x, center.y + extents.y, center.z - extents.z },
            { center.x + extents.x, center.y + extents.y, center.z - extents.z },
            { center.x - extents.x, center.y - extents.y, center.z + extents.z },
            { center.x + extents.x, center.y - extents.y, center.z + extents.z },
            { center.x - extents.x, center.y + extents.y, center.z + extents.z },
            { center.x + extents.x, center.y + extents.y, center.z + extents.z }
        };

        for (int i = 0; i < 6; ++i)
        {
            const DirectX::XMVECTOR plane = m_Planes[i];
            bool allOutside = true;

            for (int j = 0; j < 8; ++j)
            {
                const DirectX::XMFLOAT3& pt = corners[j];
                float distance = pt.x * plane.m128_f32[0] +
                    pt.y * plane.m128_f32[1] +
                    pt.z * plane.m128_f32[2] +
                    plane.m128_f32[3];

                if (distance >= 0.0f)
                {
                    allOutside = false;
                    break;
                }
            }

            if (allOutside)
                return false;
        }

        return true;
    }


    bool Frustum::CheckSphere(const DirectX::XMFLOAT3& center, float radius) const
    {
        for (int i = 0; i < 6; ++i)
        {
            const XMVECTOR plane = m_Planes[i];
            float distance = center.x * plane.m128_f32[0] +
                center.y * plane.m128_f32[1] +
                center.z * plane.m128_f32[2] +
                plane.m128_f32[3];

            if (distance < -radius)
                return false;
        }

        return true;
    }
}
