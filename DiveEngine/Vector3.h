#pragma once
#include <cmath>

namespace dive
{
	class Vector3
	{
	public:
		Vector3()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}

		Vector3(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Vector3(const Vector3& rhs)
		{
			this->x = rhs.x;
			this->y = rhs.y;
			this->z = rhs.z;
		}

		~Vector3() = default;

		// operator overloading
		void operator+=(const Vector3& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
		}

		void operator+=(const float value)
		{
			x += value;
			y += value;
			z += value;
		}

		Vector3 operator+(const Vector3& rhs) const
		{
			return Vector3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
		}

		Vector3 operator+(const float value) const
		{
			return Vector3(x + value, y + value, z + value);
		}

		void operator-=(const Vector3& rhs)
		{
			this->x -= rhs.x;
			this->y -= rhs.y;
			this->z -= rhs.z;
		}

		void operator-=(const float value)
		{
			x -= value;
			y -= value;
			z -= value;
		}

		Vector3 operator-(const Vector3& rhs) const
		{
			return Vector3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
		}

		Vector3 operator-(const float value) const
		{
			return Vector3(x - value, y - value, z - value);
		}

		void operator*=(const Vector3& rhs)
		{
			this->x *= rhs.x;
			this->y *= rhs.y;
			this->z *= rhs.z;
		}

		void operator*=(const float value)
		{
			x *= value;
			y *= value;
			z *= value;
		}

		Vector3 operator*(const Vector3& rhs) const
		{
			return Vector3(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z);
		}

		Vector3 operator*(const float value) const
		{
			return Vector3(x * value, y * value, z * value);
		}

		void operator/=(const Vector3& rhs)
		{
			this->x /= rhs.x;
			this->y /= rhs.y;
			this->z /= rhs.z;
		}

		void operator/=(const float value)
		{
			x /= value;
			y /= value;
			z /= value;
		}

		Vector3 operator/(const Vector3& rhs) const
		{
			return Vector3(this->x / rhs.x, this->y / rhs.y, this->z / rhs.z);
		}

		Vector3 operator/(const float value) const
		{
			return Vector3(x / value, y / value, z / value);
		}

		bool operator==(const Vector3& rhs)
		{
			return (this->x == rhs.x) && (this->y == rhs.y) && (this->z == rhs.z);
		}

		bool operator!=(const Vector3& rhs)
		{
			return (this->x != rhs.x) || (this->y != rhs.y) || (this->z != rhs.z);
		}

		Vector3 operator-() const { return Vector3(-x, -y, -z); }

		float Length() const { return std::sqrt(x * x + y * y + z * z); }
		float SqrLength() const { return x * x + y * y + z * z; }

		Vector3 Normalized() const
		{
			const auto sqrLength = SqrLength();

			if (sqrLength != 1.0f && sqrLength > 0.0f)
			{
				return (*this) / Length();
			}
			else
				return *this;
		}
		
		static float Dot(const Vector3& lhs, const Vector3& rhs)
		{
			return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
		}

		static Vector3 Cross(const Vector3& lhs, const Vector3& rhs)
		{
			return Vector3(
				lhs.y * rhs.z - lhs.z * rhs.y,
				lhs.z * rhs.x - lhs.x * rhs.z,
				lhs.x * rhs.y - lhs.y * rhs.x
			);
		}

		static float Angle(const Vector3& from, const Vector3& to)
		{
			// 각도를 구한다는데...
			// 결과는 오일러 degree다.
			return 0.0f;
		}

		static float Distance(const Vector3& a, const Vector3& b)
		{
			return (b - a).Length();
		}

		static Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
		{
			// 선형 보간.. 까먹었네. 젠장!!!!
			return Vector3();
		}

		static Vector3 Max(const Vector3& lhs, const Vector3& rhs)
		{
			return Vector3(fmaxf(lhs.x, rhs.x), fmaxf(lhs.y, rhs.y), fmaxf(lhs.z, rhs.z));
		}

		static Vector3 Min(const Vector3& lhs, const Vector3& rhs)
		{
			return Vector3(fminf(lhs.x, rhs.x), fminf(lhs.y, rhs.y), fminf(lhs.z, rhs.z));
		}

		static Vector3 Scale(const Vector3& a, const Vector3& b)
		{
			return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
		}

	public:
		float x;
		float y;
		float z;

		static const Vector3 Zero;
		static const Vector3 One;
		static const Vector3 Up;
		static const Vector3 Down;
		static const Vector3 Left;
		static const Vector3 Right;
		static const Vector3 Forward;
		static const Vector3 Back;
	};
}
