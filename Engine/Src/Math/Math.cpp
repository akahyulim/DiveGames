#include "DivePch.h"
#include "Math.h"

namespace Dive
{
	namespace Math
	{
		DirectX::XMFLOAT3 QuaternionToDegree(DirectX::XMFLOAT4 rotQuat)
		{
			float x, y, z, w;
			x = rotQuat.x;
			y = rotQuat.y;
			z = rotQuat.z;
			w = rotQuat.w;

			const float check = 2.0f * (-y * z + w * x);

			if (check < -0.995f)
			{
				return DirectX::XMFLOAT3(
					-90.0f,
					0.0f,
					DirectX::XMConvertToDegrees(-atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)))
				);
			}

			if (check > 0.995f)
			{
				return DirectX::XMFLOAT3(
					90.0f,
					0.0f,
					DirectX::XMConvertToDegrees(atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)))
				);
			}

			return DirectX::XMFLOAT3(
				DirectX::XMConvertToDegrees(asinf(check)),
				DirectX::XMConvertToDegrees(atan2f(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + y * y))),
				DirectX::XMConvertToDegrees(atan2f(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z)))
			);
		}

		DirectX::XMFLOAT3 QuaternionToEuler(const DirectX::XMVECTOR& quaternion)
		{
			DirectX::XMFLOAT3 euler;

			float q0 = DirectX::XMVectorGetW(quaternion);
			float q1 = DirectX::XMVectorGetX(quaternion);
			float q2 = DirectX::XMVectorGetY(quaternion);
			float q3 = DirectX::XMVectorGetZ(quaternion);

			float roll = atan2f(2.0f * (q0 * q1 + q2 * q3), 1.0f - 2.0f * (q1 * q1 + q2 * q2));
			float pitch = asinf(2.0f * (q0 * q2 - q3 * q1));
			float yaw = atan2f(2.0f * (q0 * q3 + q1 * q2), 1.0f - 2.0f * (q2 * q2 + q3 * q3));

			euler.x = DirectX::XMConvertToDegrees(roll);
			euler.y = DirectX::XMConvertToDegrees(pitch);
			euler.z = DirectX::XMConvertToDegrees(yaw);

			return euler;
		}

	}
}