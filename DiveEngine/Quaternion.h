#pragma once
#include "Vector3.h"

namespace dive
{
	class Quaternion
	{
	public:
		Quaternion()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
			w = 1.0f;
		}

		Quaternion(float x, float y, float z, float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		~Quaternion() = default;
		
		// operator overloading
		// 곱하기 자체는 꽤 복잡하다.

		Quaternion operator*(float value) const
		{
			return Quaternion(x * value, y * value, z * value, w * value);
		}

		bool operator==(const Quaternion& rhs)
		{
			return (this->x == rhs.x) && (this->y == rhs.y) && (this->z == rhs.z) && (this->w == rhs.w);
		}

		bool operator!=(const Quaternion& rhs)
		{
			return !(*this == rhs);
		}

		Vector3 GetEulerAngles() const
		{
			return Vector3();
		}

		void SetEulerAngles(const Vector3& eulerAngles)
		{

		}

		

	public:
		float x;
		float y;
		float z;
		float w;
	};
}