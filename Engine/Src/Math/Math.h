#pragma once

namespace Dive
{
	namespace Math
	{
		DirectX::XMFLOAT3 QuaternionToEularDegrees(const DirectX::XMFLOAT4& rotQuat);

		template<class T>
		T Clamp(T value, T min, T max)
		{
			if (value < min)
				return min;
			else if (value > max)
				return max;
			else
				return value;
		}
	}
}