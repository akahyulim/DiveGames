#pragma once
#include "Vector3.h"
#include "Quaternion.h"

namespace dive
{
	class Matrix4x4
	{
	public:
		Matrix4x4()
		{
			SetIdentity();
		}

		Matrix4x4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
		)
		{
			this->m00 = m00;	this->m01 = m01;	this->m02 = m02;	this->m03 = m03;
			this->m10 = m10;	this->m11 = m11;	this->m12 = m12;	this->m13 = m13;
			this->m20 = m20;	this->m21 = m21;	this->m22 = m22;	this->m23 = m23;
			this->m30 = m30;	this->m31 = m31;	this->m32 = m32;	this->m33 = m33;
		}

		Matrix4x4(const Matrix4x4& rhs)
		{
			this->m00 = rhs.m00;	this->m01 = rhs.m01;	this->m02 = rhs.m02;	this->m03 = rhs.m03;
			this->m10 = rhs.m10;	this->m11 = rhs.m11;	this->m12 = rhs.m12;	this->m13 = rhs.m13;
			this->m20 = rhs.m20;	this->m21 = rhs.m21;	this->m22 = rhs.m22;	this->m23 = rhs.m23;
			this->m30 = rhs.m30;	this->m31 = rhs.m31;	this->m32 = rhs.m32;	this->m33 = rhs.m33;

		}

		~Matrix4x4() = default;

		// operator overloading
		Matrix4x4 operator*(const Matrix4x4& rhs) const
		{
			return Matrix4x4(
				this->m00 * rhs.m00 + this->m01 * rhs.m10 + this->m02 * rhs.m20 + this->m03 * rhs.m30,
				this->m00 * rhs.m01 + this->m01 * rhs.m11 + this->m02 * rhs.m21 + this->m03 * rhs.m31,
				this->m00 * rhs.m02 + this->m01 * rhs.m12 + this->m02 * rhs.m22 + this->m03 * rhs.m32,
				this->m00 * rhs.m03 + this->m01 * rhs.m13 + this->m02 * rhs.m23 + this->m03 * rhs.m33,

				this->m10 * rhs.m00 + this->m11 * rhs.m10 + this->m12 * rhs.m20 + this->m13 * rhs.m30,
				this->m10 * rhs.m01 + this->m11 * rhs.m11 + this->m12 * rhs.m21 + this->m13 * rhs.m31,
				this->m10 * rhs.m02 + this->m11 * rhs.m12 + this->m12 * rhs.m22 + this->m13 * rhs.m32,
				this->m10 * rhs.m03 + this->m11 * rhs.m13 + this->m12 * rhs.m23 + this->m13 * rhs.m33,

				this->m20 * rhs.m00 + this->m21 * rhs.m10 + this->m22 * rhs.m20 + this->m23 * rhs.m30,
				this->m20 * rhs.m01 + this->m21 * rhs.m11 + this->m22 * rhs.m21 + this->m23 * rhs.m31,
				this->m20 * rhs.m02 + this->m21 * rhs.m12 + this->m22 * rhs.m22 + this->m23 * rhs.m32,
				this->m20 * rhs.m03 + this->m21 * rhs.m13 + this->m22 * rhs.m23 + this->m23 * rhs.m33,

				this->m30 * rhs.m00 + this->m31 * rhs.m10 + this->m32 * rhs.m20 + this->m33 * rhs.m30,
				this->m30 * rhs.m01 + this->m31 * rhs.m11 + this->m32 * rhs.m21 + this->m33 * rhs.m31,
				this->m30 * rhs.m02 + this->m31 * rhs.m12 + this->m32 * rhs.m22 + this->m33 * rhs.m32,
				this->m30 * rhs.m03 + this->m31 * rhs.m13 + this->m32 * rhs.m23 + this->m33 * rhs.m33
			);
		}

		void operator*=(const Matrix4x4 rhs)
		{
			*this = (*this) * rhs;
		}

		Vector3 operator*(const Vector3& rhs) const
		{
			float x = (rhs.x * this->m00) + (rhs.x * this->m10) + (rhs.x * this->m20) + this->m30;
			float y = (rhs.y * this->m01) + (rhs.y * this->m11) + (rhs.y * this->m21) + this->m31;
			float z = (rhs.z * this->m02) + (rhs.z * this->m12) + (rhs.z * this->m22) + this->m32;
			float w = 1 / ((rhs.x * this->m03) + (rhs.x * this->m13) + (rhs.x * this->m23) + this->m33);
	
			return Vector3(x * w, y * w, z * w);
		}

		bool operator==(const Matrix4x4& rhs)
		{
			if ((this->m00 != rhs.m00) || (this->m01 != rhs.m01) || (this->m02 != rhs.m02) || (this->m03 != rhs.m03) ||
				(this->m10 != rhs.m10) || (this->m11 != rhs.m11) || (this->m12 != rhs.m12) || (this->m13 != rhs.m13) ||
				(this->m20 != rhs.m20) || (this->m21 != rhs.m21) || (this->m22 != rhs.m22) || (this->m23 != rhs.m23) ||
				(this->m30 != rhs.m30) || (this->m31 != rhs.m31) || (this->m32 != rhs.m32) || (this->m33 != rhs.m33))
				return false;

			return true;
		}

		bool operator!=(const Matrix4x4& rhs)
		{
			return !(*this == rhs);
		}
		
		void SetIdentity()
		{
			m00 = 1.0f, m01 = 0.0f, m02 = 0.0f, m03 = 0.0f;
			m10 = 0.0f, m11 = 1.0f, m12 = 0.0f, m13 = 0.0f;
			m20 = 0.0f, m21 = 0.0f, m22 = 1.0f, m23 = 0.0f;
			m30 = 0.0f, m31 = 0.0f, m32 = 0.0f, m33 = 1.0f;
		}

		bool IsIdentity()
		{
			return (*this) == Identity;
		}

		Matrix4x4 Transpose() const
		{
			return Matrix4x4(
				m00, m10, m20, m30,
				m01, m11, m21, m31,
				m02, m12, m22, m32,
				m03, m13, m23, m33
			);
		}

		Matrix4x4 Inverse() const
		{
			// 자신의 값을 이용한 역행렬을 계산 후 리턴
			return Matrix4x4();
		}

		// 스파르탄의 경우 scale, quaternion도 얻을 수 있다.

		// static fucntion
		static Matrix4x4 Ortho(float left, float right, float bottom, float top, float near, float far)
		{
			return Matrix4x4();
		}

		static Matrix4x4 Perspective(float fov, float aspect, float near, float far)
		{
			return Matrix4x4();
		}

		static Matrix4x4 Scale(const Vector3& v)
		{
			return Matrix4x4(
				v.x,	0.0f,	0.0f,	0.0f,
				0.0f,	v.y,	0.0f,	0.0f,
				0.0f,	0.0f,	v.z,	0.0f,
				0.0f,	0.0f,	0.0f,	1.0f
			);
		}

		static Matrix4x4 Rotation(const Quaternion& q)
		{
			// 쿼터니언을 회전 행렬로 변환한다.
			return Matrix4x4();
		}

		static Matrix4x4 TRS(const Vector3& pos, const Quaternion& q, const Vector3& s)
		{
			const auto rotation = Rotation(q);

			return Matrix4x4(
				s.x * rotation.m00, s.x * rotation.m01, s.x * rotation.m02, 0.0f,
				s.y * rotation.m10, s.y * rotation.m11, s.y * rotation.m12, 0.0f,
				s.z * rotation.m20, s.z * rotation.m21, s.z * rotation.m22, 0.0f,
				pos.x,				pos.y,				pos.z,				1.0f
			);
		}

	public:
		// 1. 변수의 선언 순서대로 메모리 접근이 가능한 것 같다.
		// => 이게 좀 의심스럽다.
		// 2. hlsl에서는 행렬 순서가 다르다. 그래서 스파르탄은 순서를 달리 했다.
		// => DirectXMath의 경우엔 순서를 바꾸는 함수가 있다.
		// => 그런데 스파르탄에서 순서를 달리했다는 것은 Data()를 통해 hlsl에 전달한다는 의미인가?
		float m00 = 0.0f, m01 = 0.0f, m02 = 0.0f, m03 = 0.0f;
		float m10 = 0.0f, m11 = 0.0f, m12 = 0.0f, m13 = 0.0f;
		float m20 = 0.0f, m21 = 0.0f, m22 = 0.0f, m23 = 0.0f;
		float m30 = 0.0f, m31 = 0.0f, m32 = 0.0f, m33 = 0.0f;

		static const Matrix4x4 Identity;
		static const Matrix4x4 Zero;
	};
}