#pragma once


namespace Dive
{
	class Frustum
	{
	public:
		Frustum()	= default;
		~Frustum()	= default;

		void Construct(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, float depth);

		bool CheckPoint(const DirectX::XMVECTOR& point);
		bool CheckCube(const DirectX::XMVECTOR& center, float radius);
		bool CheckSphere(const DirectX::XMVECTOR& center, float radius);

	private:
		Frustum(const Frustum&)				= delete;
		Frustum& operator=(const Frustum&)	= delete;

	private:
		DirectX::XMFLOAT4 m_planes[6];
	};
}