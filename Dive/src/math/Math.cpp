#include "stdafx.h"
#include "Math.h"

using namespace DirectX;

namespace Dive
{
	bool Math::XMMatrixEqual(const DirectX::XMMATRIX& a, const DirectX::XMMATRIX& b)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (!XMVector4Equal(a.r[i], b.r[i]))
				return false;
		}
		return true;
	}

	bool Math::XMMatrixNearEqual(const DirectX::XMMATRIX& a, const DirectX::XMMATRIX& b, float epsilon)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (!XMVector4NearEqual(a.r[i], b.r[i], XMVectorReplicate(epsilon)))
				return false;
		}
		return true;
	}


	DirectX::XMFLOAT3 Math::CalcuBiTangent(const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangent)
	{
		const auto normalVector = XMLoadFloat3(&normal);
		const auto tangentVector = XMLoadFloat3(&tangent);

		auto biTangentVector = XMVector3Cross(normalVector, tangentVector);
		biTangentVector = XMVector3Normalize(biTangentVector);

		XMFLOAT3 biTangent;
		XMStoreFloat3(&biTangent, biTangentVector);

		return biTangent;
	}

	DirectX::XMFLOAT3 Math::QuaternionToDegrees(const DirectX::XMFLOAT4& quaternion)
	{
		auto radians = QuaternionToRadians(quaternion);

		return { 
			XMConvertToDegrees(radians.x),
			XMConvertToDegrees(radians.y),
			XMConvertToDegrees(radians.z) 
		};
	}

	DirectX::XMFLOAT4 Math::DegreesToQuaternion(const DirectX::XMFLOAT3& degrees)
	{
		XMFLOAT3 radians{
			XMConvertToRadians(degrees.x),
			XMConvertToRadians(degrees.y),
			XMConvertToRadians(degrees.z)
		};
		return RadiansToQuaternion(radians);
	}

	DirectX::XMFLOAT3 Math::QuaternionToRadians(const DirectX::XMFLOAT4& quaternion)
	{
		XMVECTOR quat = DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&quaternion));
		XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(quat);

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
		auto rotation = XMQuaternionRotationRollPitchYaw(radians.x, radians.y, radians.z);
		rotation = XMQuaternionNormalize(rotation);

		XMFLOAT4 quaternion;
		XMStoreFloat4(&quaternion, rotation);

		return quaternion;
	}

	DirectX::XMFLOAT3 Math::GetPositionFromTransform(const DirectX::XMFLOAT4X4& transform)
	{
		XMVECTOR pos, rot, scl;
		XMMatrixDecompose(&scl, &rot, &pos, XMLoadFloat4x4(&transform));

		XMFLOAT3 position;
		XMStoreFloat3(&position, pos);
		
		return position;
	}
	
	DirectX::XMFLOAT4 Math::GetRotationFromTransform(const DirectX::XMFLOAT4X4& transform)
	{
		XMVECTOR pos, rot, scl;
		XMMatrixDecompose(&scl, &rot, &pos, XMLoadFloat4x4(&transform));

		XMFLOAT4 rotation;
		XMStoreFloat4(&rotation, rot);

		return rotation;
	}
	
	DirectX::XMFLOAT3 Math::GetScaleFromTransform(const DirectX::XMFLOAT4X4& transform)
	{
		XMVECTOR pos, rot, scl;
		XMMatrixDecompose(&scl, &rot, &pos, XMLoadFloat4x4(&transform));

		XMFLOAT3 scale;
		XMStoreFloat3(&scale, scl);

		return scale;
	}
}
