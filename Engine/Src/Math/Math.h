#pragma once

namespace Dive
{
	namespace Math
	{
		bool CompareXMFLOAT4X4(const DirectX::XMFLOAT4X4& a, const DirectX::XMFLOAT4X4& b);
		bool CompareXMMATRIX(const DirectX::XMMATRIX& mat1, const DirectX::XMMATRIX& mat2);

		DirectX::XMFLOAT3 CalcuBiTangent(const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangent);

		DirectX::XMFLOAT3 QuaternionToEuler(const DirectX::XMVECTOR& quaternion);
	}
}