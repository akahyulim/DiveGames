#include "divepch.h"
#include "Frustum.h"

namespace Dive 
{
	void Frustum::Construct(const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projMatrix, float depth)
	{
        DirectX::XMFLOAT4X4 proj;
        DirectX::XMStoreFloat4x4(&proj, projMatrix);

        const float zMinimum = -proj._43 / proj._33;
        const float r = depth / (depth - zMinimum);
        proj._33 = r;
        proj._43 = -r * zMinimum;

		const auto viewProjMatrix = DirectX::XMMatrixMultiply(viewMatrix, DirectX::XMLoadFloat4x4(&proj));
        DirectX::XMFLOAT4X4 viewProj;
        DirectX::XMStoreFloat4x4(&viewProj, viewProjMatrix);

        // Left plane
        m_Planes[0].x = viewProj._14 + viewProj._11;
        m_Planes[0].y = viewProj._24 + viewProj._21;
        m_Planes[0].z = viewProj._34 + viewProj._31;
        m_Planes[0].w = viewProj._44 + viewProj._41;

        // Right plane
        m_Planes[1].x = viewProj._14 - viewProj._11;
        m_Planes[1].y = viewProj._24 - viewProj._21;
        m_Planes[1].z = viewProj._34 - viewProj._31;
        m_Planes[1].w = viewProj._44 - viewProj._41;

        // Top plane
        m_Planes[2].x = viewProj._14 - viewProj._12;
        m_Planes[2].y = viewProj._24 - viewProj._22;
        m_Planes[2].z = viewProj._34 - viewProj._32;
        m_Planes[2].w = viewProj._44 - viewProj._42;

        // Bottom plane
        m_Planes[3].x = viewProj._14 + viewProj._12;
        m_Planes[3].y = viewProj._24 + viewProj._22;
        m_Planes[3].z = viewProj._34 + viewProj._32;
        m_Planes[3].w = viewProj._44 + viewProj._42;

        // Near plane
        m_Planes[4].x = viewProj._13;
        m_Planes[4].y = viewProj._23;
        m_Planes[4].z = viewProj._33;
        m_Planes[4].w = viewProj._43;

        // Far plane
        m_Planes[5].x = viewProj._14 - viewProj._13;
        m_Planes[5].y = viewProj._24 - viewProj._23;
        m_Planes[5].z = viewProj._34 - viewProj._33;
        m_Planes[5].w = viewProj._44 - viewProj._43;

        // Normalize
        for (int i = 0; i < 6; i++)
        {
            float length = sqrtf((m_Planes[i].x * m_Planes[i].x) + (m_Planes[i].y * m_Planes[i].y) + (m_Planes[i].z * m_Planes[i].z));
            m_Planes[i].x /= length;
            m_Planes[i].y /= length;
            m_Planes[i].z /= length;
            m_Planes[i].w /= length;
        }
	}

    bool Frustum::IsVisible(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extent)
    {
        return CheckRectangle(center.x, center.y, center.z, extent.x, extent.y, extent.z);
    }

    bool Frustum::CheckPoint(float x, float y, float z) const
    {
        for (int i = 0; i < 6; i++)
        {
            if (((m_Planes[i].x * x) + (m_Planes[i].y * y) + (m_Planes[i].z * z) + m_Planes[i].w) < 0.0f)
                return false;
        }

        return true;
    }

    bool Frustum::CheckCube(float centerX, float centerY, float centerZ, float radius) const
    {
        for (int i = 0; i < 6; i++)
        {
            if (m_Planes[i].x * (centerX - radius) + m_Planes[i].y * (centerY - radius) + m_Planes[i].z * (centerZ - radius) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX + radius) + m_Planes[i].y * (centerY - radius) + m_Planes[i].z * (centerZ - radius) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX - radius) + m_Planes[i].y * (centerY + radius) + m_Planes[i].z * (centerZ - radius) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX - radius) + m_Planes[i].y * (centerY - radius) + m_Planes[i].z * (centerZ + radius) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX + radius) + m_Planes[i].y * (centerY + radius) + m_Planes[i].z * (centerZ - radius) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX + radius) + m_Planes[i].y * (centerY - radius) + m_Planes[i].z * (centerZ + radius) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX - radius) + m_Planes[i].y * (centerY + radius) + m_Planes[i].z * (centerZ + radius) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX + radius) + m_Planes[i].y * (centerY + radius) + m_Planes[i].z * (centerZ + radius) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }

            return false;
        }
        return true;
    }

    bool Frustum::CheckSphere(float centerX, float centerY, float centerZ, float radius) const
    {
        for (int i = 0; i < 6; i++)
        {
            if (((m_Planes[i].x * centerX) + (m_Planes[i].y * centerY) + (m_Planes[i].z * centerZ) + m_Planes[i].w) < -radius)
                return false;
        }

        return true;
    }

    bool Frustum::CheckRectangle(float centerX, float centerY, float centerZ, float sizeX, float sizeY, float sizeZ) const
    {
        for (int i = 0; i < 6; i++)
        {
            if (m_Planes[i].x * (centerX - sizeX) + m_Planes[i].y * (centerY - sizeY) + m_Planes[i].z * (centerZ - sizeZ) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX + sizeX) + m_Planes[i].y * (centerY - sizeY) + m_Planes[i].z * (centerZ - sizeZ) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX - sizeX) + m_Planes[i].y * (centerY + sizeY) + m_Planes[i].z * (centerZ - sizeZ) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX - sizeX) + m_Planes[i].y * (centerY - sizeY) + m_Planes[i].z * (centerZ + sizeZ) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX + sizeX) + m_Planes[i].y * (centerY - sizeY) + m_Planes[i].z * (centerZ + sizeZ) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX + sizeX) + m_Planes[i].y * (centerY + sizeY) + m_Planes[i].z * (centerZ - sizeZ) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX - sizeX) + m_Planes[i].y * (centerY + sizeY) + m_Planes[i].z * (centerZ + sizeZ) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }
            if (m_Planes[i].x * (centerX + sizeX) + m_Planes[i].y * (centerY + sizeY) + m_Planes[i].z * (centerZ + sizeZ) + m_Planes[i].w >= 0.0f)
            {
                continue;
            }

            return false;
        }

        return true;
    }
}