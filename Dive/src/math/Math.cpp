#include "stdafx.h"
#include "Math.h"

namespace Dive
{
	bool Math::CompareXMFLOAT4X4(const DirectX::XMFLOAT4X4& a, const DirectX::XMFLOAT4X4& b)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (a.m[i][j] != b.m[i][j])
				{
					return false;
				}
			}
		}

		return true;
	}

	bool Math::CompareXMMATRIX(const DirectX::XMMATRIX& mat1, const DirectX::XMMATRIX& mat2)
	{
		DirectX::XMFLOAT4X4 a, b;
		DirectX::XMStoreFloat4x4(&a, mat1);
		DirectX::XMStoreFloat4x4(&b, mat2);

		return CompareXMFLOAT4X4(a, b);
	}

	DirectX::XMFLOAT3 Math::CalcuBiTangent(const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangent)
	{
		const auto normalVector = DirectX::XMLoadFloat3(&normal);
		const auto tangentVector = DirectX::XMLoadFloat3(&tangent);

		auto biTangentVector = DirectX::XMVector3Cross(normalVector, tangentVector);
		biTangentVector = DirectX::XMVector3Normalize(biTangentVector);

		DirectX::XMFLOAT3 biTangent;
		DirectX::XMStoreFloat3(&biTangent, biTangentVector);

		return biTangent;
	}

	DirectX::XMFLOAT3 Math::QuaternionToDegrees(const DirectX::XMFLOAT4& quaternion)
	{
		DirectX::XMFLOAT3 degrees{};

		float q0 = quaternion.w;
		float q1 = quaternion.x;
		float q2 = quaternion.y;
		float q3 = quaternion.z;

		float roll = atan2f(2.0f * (q0 * q1 + q2 * q3), 1.0f - 2.0f * (q1 * q1 + q2 * q2));
		float pitch = asinf(std::clamp(2.0f * (q0 * q2 - q3 * q1), -1.0f, 1.0f));
		float yaw = atan2f(2.0f * (q0 * q3 + q1 * q2), 1.0f - 2.0f * (q2 * q2 + q3 * q3));

		degrees.x = DirectX::XMConvertToDegrees(roll);
		degrees.y = DirectX::XMConvertToDegrees(pitch);
		degrees.z = DirectX::XMConvertToDegrees(yaw);

		return degrees;
	}
}
