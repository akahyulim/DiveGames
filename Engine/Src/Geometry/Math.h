#pragma once
#include "DivePch.h"

namespace Dive
{
	namespace Math
	{
		// ��ġ, ȸ��, ũ�� ��ȯ �����κ��� ��ȯ ��� �����
		DirectX::XMMATRIX CalcurateTransformMatrix(const DirectX::XMVECTOR& pos, const DirectX::XMVECTOR& rot, const DirectX::XMVECTOR& scl)
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMStoreFloat3(&position, pos);

			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, scl);

			// ȸ�� ������� ȸ�� ��� �����
			DirectX::XMFLOAT4X4 rotationMatrix;
			DirectX::XMStoreFloat4x4(&rotationMatrix, DirectX::XMMatrixRotationQuaternion(rot));

			// ���� ��
			DirectX::XMFLOAT4X4 mat;
			mat._11 = scale.x * rotationMatrix._11; mat._12 = scale.x * rotationMatrix._12; mat._13 = scale.x * rotationMatrix._13; mat._14 = 0.0f;
			mat._21 = scale.y * rotationMatrix._21; mat._22 = scale.y * rotationMatrix._22; mat._23 = scale.y * rotationMatrix._23; mat._24 = 0.0f;
			mat._31 = scale.z * rotationMatrix._31; mat._32 = scale.z * rotationMatrix._32; mat._33 = scale.z * rotationMatrix._33; mat._34 = 0.0f;
			mat._41 = position.x;					mat._42 = position.y;					mat._43 = position.z;					mat._44 = 1.0f;

			return DirectX::XMLoadFloat4x4(&mat);
		}

		// ��ȯ ��Ŀ��� Scale vector �̱�
		// �ϴ� ���忡 ������ ������� �ʴ´�.
		DirectX::XMVECTOR GetScale(const DirectX::XMMATRIX& mat)
		{
			DirectX::XMFLOAT4X4 m;
			DirectX::XMStoreFloat4x4(&m, mat);

			// 4��° ����?
			// ������ �������� �ʴ´ٸ� �� �κ��� �ʿ������?
			int x = 1; //((m._11 * m._12 * m._13 * m._14) < 0) ? -1 : 1;
			int y = 1; //((m._21 * m._22 * m._23 * m._24) < 0) ? -1 : 1;
			int z = 1; //((m._31 * m._32 * m._33 * m._34) < 0) ? -1 : 1;

			// ���� ũ��?�� ��ȣ�� ���ߴ�.
			DirectX::XMFLOAT3 scale;
			scale.x = static_cast<float>(x) * sqrtf(m._11 * m._11 + m._12 * m._12 + m._13 * m._13);
			scale.y = static_cast<float>(y) * sqrtf(m._21 * m._21 + m._22 * m._22 + m._23 * m._23);
			scale.z = static_cast<float>(z) * sqrtf(m._31 * m._31 + m._32 * m._32 + m._33 * m._33);

			return DirectX::XMLoadFloat3(&scale);
		}

		// ��ȯ ��Ŀ��� ȸ�� ����� �̱�
		DirectX::XMVECTOR GetRotationQuaternion(const DirectX::XMMATRIX& mat, const DirectX::XMVECTOR& scl)
		{
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, scl);

			if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f)
				return DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			DirectX::XMFLOAT4X4 m;
			DirectX::XMStoreFloat4x4(&m, mat);

			// Scale�� ���� �����ش�.
			m._11 /= scale.x;	m._12 /= scale.x;	m._13 /= scale.x;	m._14 = 0.0f;
			m._21 /= scale.y;	m._22 /= scale.y;	m._23 /= scale.y;	m._24 = 0.0f;
			m._31 /= scale.z;	m._32 /= scale.z;	m._33 /= scale.z;	m._34 = 0.0f;
			m._41 = 0.0f;		m._42 = 0.0f;		m._43 = 0.0f;		m._44 = 1.0f;

			return DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&m));
		}

		// ȸ�� ������� angle�� ��ȯ 
		DirectX::XMVECTOR QuaternionConvertDegrees(DirectX::XMVECTOR quaternion)
		{
			DirectX::XMFLOAT4 rot;
			DirectX::XMStoreFloat4(&rot, quaternion);

			// Derivation from http://www.geometrictools.com/Documentation/EulerAngles.pdf
			// Order of rotations: Z first, then X, then Y
			float check = 2.0f * (-rot.y * rot.z + rot.w * rot.x);

			if (check < -0.995f)
			{
				return DirectX::XMVectorSet(
					-90.0f,
					0.0f,
					-atan2f(2.0f * (rot.x * rot.z - rot.w * rot.y), 1.0f - 2.0f * (rot.y * rot.y + rot.z * rot.z)) * (180.0f / 3.14159265359f),
					1.0f
				);
			}
			
			if (check > 0.995f)
			{
				return DirectX::XMVectorSet(
					90.0f,
					0.0f,
					atan2f(2.0f * (rot.x * rot.z - rot.w * rot.y), 1.0f - 2.0f * (rot.y * rot.y + rot.z * rot.z)) * (180.0f / 3.14159265359f),
					1.0f
				);

			}

			return DirectX::XMVectorSet(
				asinf(check) * (180.0f / 3.14159265359f),
				atan2f(2.0f * (rot.x * rot.z + rot.w * rot.y), 1.0f - 2.0f * (rot.x * rot.x + rot.y * rot.y)) * (180.0f / 3.14159265359f),
				atan2f(2.0f * (rot.x * rot.y + rot.w * rot.z), 1.0f - 2.0f * (rot.x * rot.x + rot.z * rot.z)) * (180.0f / 3.14159265359f),
				1.0f
			);
		}
	}
}