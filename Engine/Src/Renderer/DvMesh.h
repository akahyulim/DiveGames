#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class DvVertexBuffer;
	class DvIndexBuffer;

	class DvMesh : public Resource
	{
	public:
		// ������ Ȥ�� SetMacro���� �Լ��� ��ũ�θ� ������ Input Type�� �پ��ϰ� ���� �� �ִ�.
		// �� ���� Ÿ�� ������ ������ŭ ���̴��� �߰��� �����ȴٴ� �ǹ̴�.
		// ���̴� ���ο����� ����� ���������� ���̴� ��ü�� ������ ��ư� �ȴ�.
		DvMesh(DvVertexBuffer* pVertexBuffer, DvIndexBuffer* pIndexBuffer);	// �����?
		DvMesh(const std::string& name, DvVertexBuffer* pVertexBuffer, DvIndexBuffer* pIndexBuffer);
		~DvMesh();

		void Draw(D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	public:
		static DvMesh* Create(const std::string& name, DvVertexBuffer* pVertexBuffer, DvIndexBuffer* pIndexBuffer);

	private:
	private:
		DvVertexBuffer* m_pVertexBuffer;
		DvIndexBuffer* m_pIndexBuffer;
	};
}