#pragma once
#include <cmath>

namespace dive
{
	class Vector2
	{
	public:
		Vector2()
		{
			x = 0.0f;
			y = 0.0f;
		}

		Vector2(float x, float y)
		{
			this->x = x;
			this->y = y;
		}

		Vector2(const Vector2& rhs)
		{
			this->x = rhs.x;
			this->y = rhs.y;
		}

		~Vector2() = default;

		void operator+=(const Vector2& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
		}

		void operator+=(const float value)
		{
			x += value;
			y += value;
		}

		Vector2 operator+(const Vector2& rhs) const
		{
			return Vector2(this->x + rhs.x, this->y + rhs.y);
		}

		Vector2 operator+(const float value) const
		{
			return Vector2(x + value, y + value);
		}

		void operator-=(const Vector2& rhs)
		{
			this->x -= rhs.x;
			this->y -= rhs.y;
		}

		void operator-=(const float value)
		{
			x -= value;
			y -= value;
		}

		Vector2 operator-(const Vector2& rhs) const
		{
			return Vector2(this->x - rhs.x, this->y - rhs.y);
		}

		Vector2 operator-(const float value) const
		{
			return Vector2(x - value, y - value);
		}

		void operator*=(const Vector2& rhs)
		{
			this->x *= rhs.x;
			this->y *= rhs.y;
		}

		void operator*=(const float value)
		{
			x *= value;
			y *= value;
		}

		Vector2 operator*(const Vector2& rhs) const
		{
			return Vector2(this->x * rhs.x, this->y * rhs.y);
		}

		Vector2 operator*(const float value) const
		{
			return Vector2(x * value, y * value);
		}

		void operator/=(const Vector2& rhs)
		{
			this->x /= rhs.x;
			this->y /= rhs.y;
		}

		void operator/=(const float value)
		{
			x /= value;
			y /= value;
		}

		Vector2 operator/(const Vector2& rhs) const
		{
			return Vector2(this->x / rhs.x, this->y / rhs.y);
		}

		Vector2 operator/(const float value) const
		{
			return Vector2(x / value, y / value);
		}

		bool operator==(const Vector2& rhs)
		{
			return (this->x == rhs.x) && (this->y == rhs.y);
		}

		bool operator!=(const Vector2& rhs)
		{
			return (this->x != rhs.x) || (this->y != rhs.y);
		}

		Vector2 operator-() const { return Vector2(-x, -y); }

		float Length() const { return std::sqrt(x * x + y * y); }
		float SqrLenth() const { return x * x + y * y; }


		// Nomalize
		// Nomalized

	public:
		float x;
		float y;

		static const Vector2 Zero;
		static const Vector2 One;
		static const Vector2 Up;
		static const Vector2 Down;
		static const Vector2 Left;
		static const Vector2 Right;
	};
}