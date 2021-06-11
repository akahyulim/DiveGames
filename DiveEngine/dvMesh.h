#pragma once
#include "GraphicsInclude.h"
#include "Resource.h"
#include <vector>
#include <string>

namespace Dive
{	
	class GameObject;

	enum eMeshType
	{
		Static,
		Skinned,
	};

	// static, skinned �� ������ �����Ѵ�. => �𸮾� ���̴�.
	// renderable���� buffer�� �����ϱ� ���� �� ��ü�� �����Ѵ�.
	// ���� Resource�� �ƹ��͵� ����. ���� ���� ������ �̾Ƴ���.
	// => ������ ���縦 �ذ����� ���ߴ�. �׸��� Instancing ���� ���ε� �����ؾ� �Ѵ�.
	class dvMesh : public Resource
	{
	public:
		dvMesh();
		~dvMesh();

		bool SaveToFile(const std::string& filepath);			// Engine Format���� �����Ѵ�.
		bool LoadFromFile(const std::string& filepath);			// Engine Format�� �ε��Ѵ�. �� �������� GameObject�� �����ؾ� �Ѵ�.

		bool CreateGeometryBuffer();

		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }

		const eMeshType& GetMeshType() const { return m_type; }
		unsigned int GetVertexCount() const { return static_cast<unsigned int>(m_positions.size()); }
		unsigned int GetIndexCount() const { return static_cast<unsigned int>(m_indices.size()); }

	private:
		// ���۸� ������ �� �� ��ҵ��� ���� ����ؾ� �Ѵ�.

	public:
		std::vector<DirectX::XMFLOAT3> m_positions;
		std::vector<DirectX::XMFLOAT2> m_uv;
		std::vector<DirectX::XMFLOAT2> m_uv1;
		std::vector<DirectX::XMFLOAT4> m_colors;
		std::vector<DirectX::XMFLOAT3> m_normals;
		std::vector<DirectX::XMFLOAT3> m_tangents;
		// bone ���θ� ���� static�� skinned�� ���еȴ�.
		std::vector<unsigned int> m_indices;

	private:
		GameObject* m_pRoot;	// �� �ʿ��ϴ�.

		std::string m_name;
		eMeshType m_type;

		// �� ��� Buffer class�� �����...
		unsigned int m_stride;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
	};
}