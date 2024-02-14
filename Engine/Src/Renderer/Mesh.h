#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;

	class Mesh : public Resource
	{
	public:
		// ������ Ȥ�� SetMacro���� �Լ��� ��ũ�θ� ������ Input Type�� �پ��ϰ� ���� �� �ִ�.
		// �� ���� Ÿ�� ������ ������ŭ ���̴��� �߰��� �����ȴٴ� �ǹ̴�.
		// ���̴� ���ο����� ����� ���������� ���̴� ��ü�� ������ ��ư� �ȴ�.
		Mesh(VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer);	// �����?
		Mesh(const std::string& name, VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer);
		~Mesh();

		void Draw(D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	public:
		static Mesh* Create(const std::string& name, VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer);

	private:
	private:
		VertexBuffer* m_pVertexBuffer;
		IndexBuffer* m_pIndexBuffer;
	};
}