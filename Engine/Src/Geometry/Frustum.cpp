#include "DivePch.h"
#include "Frustum.h"


namespace Dive
{
	void Frustum::Construct(const DirectX::XMMATRIX & view, const DirectX::XMMATRIX & proj, float depth)
	{
		// �� ���
		// �� ���
		// �� ���
		// �� ���
		// �� ���
		// �� ���
	}

	bool Frustum::CheckPoint(const DirectX::XMVECTOR & point)
	{
		for (const auto& plane : m_planes)
		{

		}

		return false;
	}
	
	bool Frustum::CheckCube(const DirectX::XMVECTOR & center, float radius)
	{
		return false;
	}
	
	bool Frustum::CheckSphere(const DirectX::XMVECTOR & center, float radius)
	{
		return false;
	}
}