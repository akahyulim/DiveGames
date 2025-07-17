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
		DirectX::XMFLOAT3 radians{
			DirectX::XMConvertToRadians(degrees.x),
			DirectX::XMConvertToRadians(degrees.y),
			DirectX::XMConvertToRadians(degrees.z)
		};
		return RadiansToQuaternion(radians);
	}

	DirectX::XMFLOAT3 Math::QuaternionToRadians(const DirectX::XMFLOAT4& quaternion)
	{
		DirectX::XMVECTOR quat = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&quaternion));
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(quat);

		float pitch = std::atan2(rotationMatrix.r[1].m128_f32[2], rotationMatrix.r[2].m128_f32[2]);
		float yaw = std::atan2(-rotationMatrix.r[0].m128_f32[2],
			std::sqrt(rotationMatrix.r[1].m128_f32[2] * rotationMatrix.r[1].m128_f32[2] +
				rotationMatrix.r[2].m128_f32[2] * rotationMatrix.r[2].m128_f32[2]));
		float roll = std::atan2(rotationMatrix.r[0].m128_f32[1], rotationMatrix.r[0].m128_f32[0]);

		pitch = (std::fabs(pitch) < 1e-6) ? 0.0f : pitch;
		yaw = (std::fabs(yaw) < 1e-6) ? 0.0f : yaw;
		roll = (std::fabs(roll) < 1e-6) ? 0.0f : roll;

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

	DirectX::XMFLOAT3 Math::GetPositionFromTransform(const DirectX::XMFLOAT4X4& transform)
	{
		DirectX::XMVECTOR pos, rot, scl;
		DirectX::XMMatrixDecompose(&scl, &rot, &pos, DirectX::XMLoadFloat4x4(&transform));

		DirectX::XMFLOAT3 position;
		DirectX::XMStoreFloat3(&position, pos);
		
		return position;
	}
	
	DirectX::XMFLOAT4 Math::GetRotationFromTransform(const DirectX::XMFLOAT4X4& transform)
	{
		DirectX::XMVECTOR pos, rot, scl;
		DirectX::XMMatrixDecompose(&scl, &rot, &pos, DirectX::XMLoadFloat4x4(&transform));

		DirectX::XMFLOAT4 rotation;
		DirectX::XMStoreFloat4(&rotation, rot);

		return rotation;
	}
	
	DirectX::XMFLOAT3 Math::GetScaleFromTransform(const DirectX::XMFLOAT4X4& transform)
	{
		DirectX::XMVECTOR pos, rot, scl;
		DirectX::XMMatrixDecompose(&scl, &rot, &pos, DirectX::XMLoadFloat4x4(&transform));

		DirectX::XMFLOAT3 scale;
		DirectX::XMStoreFloat3(&scale, scl);

		return scale;
	}
}
