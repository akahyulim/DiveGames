#pragma once

namespace Dive
{
	class Frustum
	{
	public:
		Frustum() = default;
		~Frustum() = default;

		void Construct(const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projMatrix, float depth);

		// Spratan은 IsVisible에서 center와 extend 두 개의 vector3 값을 전달받고
		// CheckCube로 확인한다.
		// 그리고 center과 extend는 Renderable에서 BoundingBox로부터 얻는다.
		bool IsVisible(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extent);
		bool CheckPoint(float x, float y, float z) const;
		bool CheckCube(float centerX, float centerY, float centerZ, float radius) const;
		bool CheckSphere(float centerX, float centerY, float centerZ, float radius) const;
		bool CheckRectangle(float centerX, float centerY, float centerZ, float sizeX, float sizeY, float sizeZ) const;

	private:
		DirectX::XMFLOAT4 m_Planes[6];
	};
}
