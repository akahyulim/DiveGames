#pragma once
#include "Vector3.h"

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

		// ��� ������ ����� �ٸ���. 
		// ������ ��ȯ? ������ �� ����. �� �� �˾ƺ� �� ��������.
		Vector3 operator*(const Vector3& rhs) const
		{
			float x = (rhs.x * this->m00) + (rhs.x * this->m10) + (rhs.x * this->m20) + this->m30;
			float y = (rhs.y * this->m01) + (rhs.y * this->m11) + (rhs.y * this->m21) + this->m31;
			float z = (rhs.z * this->m02) + (rhs.z * this->m12) + (rhs.z * this->m22) + this->m32;
			//float w = (rhs.x * this->m00) + (rhs.x * this->m10) + (rhs.x * this->m20) + this->m30;
	
			return Vector3();
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

		// Inverse : ����� ����
		// IsIdentity : ���� ��� Ȯ��
		// Transpose : ��ġ ��� ��ȯ

		// ���ĸ�ź�� ��� scale, quaternion�� ���� �� �ִ�.

		// static fucntion
		// Ortho : left, right, bottom, top, near, far�� �����ϸ� ���� ���� ��� ���� ����
		// Perspective : fov, aspect, near, far�� �����ϸ� ���� ��� ���� ����
		static Matrix4x4 Scale(const Vector3& v)
		{
			return Matrix4x4(
				v.x,	0.0f,	0.0f,	0.0f,
				0.0f,	v.y,	0.0f,	0.0f,
				0.0f,	0.0f,	v.z,	0.0f,
				0.0f,	0.0f,	0.0f,	1.0f
			);
		}

		// TRS: Vector3 pos, Quaternion q, Vectror3 s�� �����ϸ� ��ȯ ��� ���� ����

	public:
		// 1. ������ ���� ������� �޸� ������ ������ �� ����.
		// => �̰� �� �ǽɽ�����.
		// 2. hlsl������ ��� ������ �ٸ���. �׷��� ���ĸ�ź�� ������ �޸� �ߴ�.
		// => DirectXMath�� ��쿣 ������ �ٲٴ� �Լ��� �ִ�.
		// => �׷��� ���ĸ�ź���� ������ �޸��ߴٴ� ���� Data()�� ���� hlsl�� �����Ѵٴ� �ǹ��ΰ�?
		float m00 = 0.0f, m01 = 0.0f, m02 = 0.0f, m03 = 0.0f;
		float m10 = 0.0f, m11 = 0.0f, m12 = 0.0f, m13 = 0.0f;
		float m20 = 0.0f, m21 = 0.0f, m22 = 0.0f, m23 = 0.0f;
		float m30 = 0.0f, m31 = 0.0f, m32 = 0.0f, m33 = 0.0f;

		static const Matrix4x4 Identity;
		static const Matrix4x4 Zero;
	};
}