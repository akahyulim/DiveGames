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
		auto radians = QuaternionToRadians(quaternion);

		return { 
			DirectX::XMConvertToDegrees(radians.x),
			DirectX::XMConvertToDegrees(radians.y),
			DirectX::XMConvertToDegrees(radians.z) 
		};
	}

	DirectX::XMFLOAT4 Math::DegreesToQuaternion(const DirectX::XMFLOAT3& degrees)
	{
		DirectX::XMFLOAT3 radians;
		radians.x = DirectX::XMConvertToRadians(degrees.x);
		radians.y = DirectX::XMConvertToRadians(degrees.y);
		radians.z = DirectX::XMConvertToRadians(degrees.z);

		return RadiansToQuaternion(radians);
	}

	DirectX::XMFLOAT3 Math::QuaternionToRadians(const DirectX::XMFLOAT4& quaternion)
	{
		auto quat = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&quaternion));

		float qw = DirectX::XMVectorGetW(quat);
		float qx = DirectX::XMVectorGetX(quat);
		float qy = DirectX::XMVectorGetY(quat);
		float qz = DirectX::XMVectorGetZ(quat);

		float pitch = std::asin(std::clamp(2.0f * (qx * qy + qw * qz), -1.0f, 1.0f));
		float yaw = std::atan2(2.0f * (qw * qy - qz * qx), 1.0f - 2.0f * (qy * qy + qz * qz));
		float roll = std::atan2(2.0f * (qw * qx - qy * qz), 1.0f - 2.0f * (qx * qx + qz * qz));

		return { pitch, yaw, roll };
	}

	DirectX::XMFLOAT4 Math::RadiansToQuaternion(const DirectX::XMFLOAT3& radians)
	{
		auto rotation = DirectX::XMQuaternionRotationRollPitchYaw(radians.x, radians.y, radians.z);
		rotation = DirectX::XMQuaternionNormalize(rotation);

		DirectX::XMFLOAT4 quaternion;
		DirectX::XMStoreFloat4(&quaternion, rotation);

		return quaternion;
	}
}
