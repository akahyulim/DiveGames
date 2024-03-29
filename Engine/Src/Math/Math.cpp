#include "DivePch.h"
#include "Math.h"

namespace Dive
{
	namespace Math
	{
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