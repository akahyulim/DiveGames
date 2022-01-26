#pragma once
#include "Renderer/Graphics/GraphicsInclude.h"
#include <algorithm>

#define saturate(x) std::min(std::max(x,0.0f),1.0f)

// 추후 class로 구현할 거리가 생기면
// Math 폴더로 독립시키고 Math namespace로 일관성을 유지하자.
namespace Dive::Math
{
	inline DirectX::XMFLOAT3 QuaternionToEulerAngles(const DirectX::XMFLOAT4& quaternion)
	{
		float x, y, z, w;
		x = quaternion.x;
		y = quaternion.y;
		z = quaternion.z;
		w = quaternion.w;

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