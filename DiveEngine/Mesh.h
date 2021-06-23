#pragma once
#include "GraphicsInclude.h"
#include "GraphicsDevice.h"
#include <vector>
#include <memory>

namespace dive
{
	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh() = default;

		bool CreateRenderDate();

		// �̿ܿ��� normal�� ����ϴ� �� �߰� ��� ���� �Լ��� �ʿ��ϴ�.

		unsigned int GetVertexCount() const { return m_vertexCount; }
		unsigned int GetIndexCount() const { return m_indexCount; }

		//DXGI_FORMAT GetIndexFormat() { return m_positions.size() > 65535 ?  }
		// normal ��� - �� ���� ��� �� ����

	private:

	public:
		std::vector<DirectX::XMFLOAT3> m_positions;
		std::vector<DirectX::XMFLOAT2> m_uv;
		std::vector<DirectX::XMFLOAT2> m_uv1;
		std::vector<DirectX::XMFLOAT4> m_colors;
		std::vector<DirectX::XMFLOAT3> m_normals;
		std::vector<DirectX::XMFLOAT3> m_tangents;

		std::vector<unsigned int> m_indices;

	
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVBPosition;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVBUv;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVBUv1;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVBColor;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVBNormal;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVBTangent;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIB;

		unsigned int m_vertexCount = 0;
		unsigned int m_indexCount = 0;

		unsigned int m_vertexBufferCount = 0;
		unsigned int m_vertexBufferStride = 0;

		// bind index buffer���� format�� �޴´�. �ٵ� ���� 16bit�� �� �ʿ䰡 �ֳ� �ʹ�.
	};
}