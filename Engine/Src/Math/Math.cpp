#include "divepch.h"
#include "Math.h"

namespace Dive
{
	namespace Math
	{
		DirectX::XMFLOAT3 QuaternionToEularDegrees(DirectX::XMFLOAT4 rotQuat)
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
	}
}