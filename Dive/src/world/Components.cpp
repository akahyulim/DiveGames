#include "stdafx.h"
#include "components.h"

namespace Dive
{
	void LocalTransform::FromMatrix(const DirectX::XMFLOAT4X4& matrix)
	{
		DirectX::XMVECTOR pos, rot, scl;
		DirectX::XMMatrixDecompose(&scl, &rot, &pos, DirectX::XMLoadFloat4x4(&matrix));

		DirectX::XMStoreFloat3(&Position, pos);
		DirectX::XMStoreFloat4(&Rotation, rot);
		DirectX::XMStoreFloat3(&Scale, scl);
	}

	void LocalTransform::Translate(const DirectX::XMFLOAT3& translation)
	{
		DirectX::XMVECTOR vecTranslation = DirectX::XMLoadFloat3(&translation);
		DirectX::XMVECTOR rotatedTranslation = DirectX::XMVector3Rotate(vecTranslation, DirectX::XMLoadFloat4(&Rotation));

		DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&Position);
		pos = DirectX::XMVectorAdd(pos, rotatedTranslation);

		DirectX::XMStoreFloat3(&Position, pos);
	}

	void LocalTransform::Rotate(const DirectX::XMFLOAT4& quaternion)
	{
		DirectX::XMVECTOR vecQuaternion = DirectX::XMLoadFloat4(&quaternion);
		DirectX::XMVECTOR vecRotation = DirectX::XMLoadFloat4(&Rotation);

		vecRotation = DirectX::XMQuaternionMultiply(vecQuaternion, vecRotation);
		vecRotation = DirectX::XMQuaternionNormalize(vecRotation);

		DirectX::XMStoreFloat4(&Rotation, vecRotation);
	}

	void LocalTransform::RotateX(float angle)
	{
		DirectX::XMVECTOR quatRotation = DirectX::XMQuaternionRotationRollPitchYaw(angle, 0.0f, 0.0f);
		DirectX::XMVECTOR vecRotation = DirectX::XMLoadFloat4(&Rotation);

		vecRotation = DirectX::XMQuaternionMultiply(quatRotation, vecRotation);
		vecRotation = DirectX::XMQuaternionNormalize(vecRotation);

		DirectX::XMStoreFloat4(&Rotation, vecRotation);
	}

	void LocalTransform::RotateY(float angle)
	{
		DirectX::XMVECTOR quatRotation = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, angle, 0.0f);
		DirectX::XMVECTOR vecRotation = DirectX::XMLoadFloat4(&Rotation);

		vecRotation = DirectX::XMQuaternionMultiply(quatRotation, vecRotation);
		vecRotation = DirectX::XMQuaternionNormalize(vecRotation);

		DirectX::XMStoreFloat4(&Rotation, vecRotation);
	}

	void LocalTransform::RotateZ(float angle)
	{
		DirectX::XMVECTOR quatRotation = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, angle);
		DirectX::XMVECTOR vecRotation = DirectX::XMLoadFloat4(&Rotation);

		vecRotation = DirectX::XMQuaternionMultiply(quatRotation, vecRotation);
		vecRotation = DirectX::XMQuaternionNormalize(vecRotation);

		DirectX::XMStoreFloat4(&Rotation, vecRotation);
	}

	DirectX::XMFLOAT3 LocalTransform::Forward() const
	{
		DirectX::XMVECTOR vecForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		vecForward = DirectX::XMVector3Rotate(vecForward, DirectX::XMLoadFloat4(&Rotation));

		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, vecForward);
		return result;
	}

	DirectX::XMFLOAT3 LocalTransform::Up() const
	{
		DirectX::XMVECTOR vecUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		vecUp = DirectX::XMVector3Rotate(vecUp, DirectX::XMLoadFloat4(&Rotation));

		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, vecUp);
		return result;
	}

	DirectX::XMFLOAT3 LocalTransform::Right() const
	{
		DirectX::XMVECTOR vecRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		vecRight = DirectX::XMVector3Rotate(vecRight, DirectX::XMLoadFloat4(&Rotation));

		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, vecRight);
		return result;
	}
}