#pragma once

namespace Dive
{
	class Frustum
	{
	public:
		Frustum() = default;
		~Frustum() = default;

		void Construct(const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projMatrix, float depth);

		// Spratan�� IsVisible���� center�� extend �� ���� vector3 ���� ���޹ް�
		// CheckCube�� Ȯ���Ѵ�.
		// �׸��� center�� extend�� Renderable���� BoundingBox�κ��� ��´�.
		bool IsVisible(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extent);
		bool CheckPoint(float x, float y, float z) const;
		bool CheckCube(float centerX, float centerY, float centerZ, float radius) const;
		bool CheckSphere(float centerX, float centerY, float centerZ, float radius) const;
		bool CheckRectangle(float centerX, float centerY, float centerZ, float sizeX, float sizeY, float sizeZ) const;

	private:
		DirectX::XMFLOAT4 m_Planes[6];
	};
}