#include "stdafx.h"
#include "Frustum.h"

using namespace DirectX;

namespace Dive 
{
    void Frustum::Update(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& proj)
    {
        XMMATRIX viewMat = XMLoadFloat4x4(&view);
        XMMATRIX projMat = XMLoadFloat4x4(&proj);
        XMMATRIX viewProj = XMMatrixMultiply(viewMat, projMat);
        XMMATRIX m = XMMatrixTranspose(viewProj);

        m_Planes[0] = XMPlaneNormalize(m.r[3] + m.r[0]); // Left
        m_Planes[1] = XMPlaneNormalize(m.r[3] - m.r[0]); // Right
        m_Planes[2] = XMPlaneNormalize(m.r[3] - m.r[1]); // Top
        m_Planes[3] = XMPlaneNormalize(m.r[3] + m.r[1]); // Bottom
        m_Planes[4] = XMPlaneNormalize(m.r[2]);          // Near
        m_Planes[5] = XMPlaneNormalize(m.r[3] - m.r[2]); // Far
    }

    bool Frustum::CheckAABB(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents) const
    {
        for (int i = 0; i < 6; ++i)
        {
            const XMVECTOR plane = m_Planes[i];
            const XMVECTOR normal = XMVectorSet(plane.m128_f32[0], plane.m128_f32[1], plane.m128_f32[2], 0.0f);
            const float d = plane.m128_f32[3];

            float x = (plane.m128_f32[0] < 0.0f) ? center.x + extents.x : center.x - extents.x;
            float y = (plane.m128_f32[1] < 0.0f) ? center.y + extents.y : center.y - extents.y;
            float z = (plane.m128_f32[2] < 0.0f) ? center.z + extents.z : center.z - extents.z;

            float distance = x * plane.m128_f32[0] + y * plane.m128_f32[1] + z * plane.m128_f32[2] + d;

            if (distance < 0.0f)
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
