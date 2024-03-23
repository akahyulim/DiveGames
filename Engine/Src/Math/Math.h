#pragma once

namespace Dive
{
	namespace Math
	{
		DirectX::XMFLOAT3 QuaternionToDegree(DirectX::XMFLOAT4 rotQuat);
		DirectX::XMFLOAT3 QuaternionToEuler(const DirectX::XMVECTOR& quaternion);
	}
}