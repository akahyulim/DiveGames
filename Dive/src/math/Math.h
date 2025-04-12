#pragma once

namespace Dive
{
	class Math
	{
	public:
		static bool CompareXMFLOAT4X4(const DirectX::XMFLOAT4X4& a, const DirectX::XMFLOAT4X4& b);
		static bool CompareXMMATRIX(const DirectX::XMMATRIX& mat1, const DirectX::XMMATRIX& mat2);

		static DirectX::XMFLOAT3 CalcuBiTangent(const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangent);
		static DirectX::XMFLOAT3 QuaternionToDegrees(const DirectX::XMFLOAT4& quaternion);
	};
}
