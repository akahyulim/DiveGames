#pragma once
#include "Dive.h"
#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>

class AssimpHelpers
{
public:
    static DirectX::XMFLOAT4X4 ConvertMatrix(const aiMatrix4x4& transform)
    {
        return DirectX::XMFLOAT4X4
        (
            transform.a1, transform.b1, transform.c1, transform.d1,
            transform.a2, transform.b2, transform.c2, transform.d2,
            transform.a3, transform.b3, transform.c3, transform.d3,
            transform.a4, transform.b4, transform.c4, transform.d4
        );
    }

    static DirectX::XMFLOAT3 VectorConvertFloat3(const aiVector3D& vector)
    {
        return DirectX::XMFLOAT3
        {
            vector.x, vector.y, vector.z
        };
    }

    static DirectX::XMFLOAT4 QuaternionConvertFloat4(const aiQuaternion& quat)
    {
        return DirectX::XMFLOAT4
        {
            quat.x, quat.y, quat.z, quat.w
        };
    }
};