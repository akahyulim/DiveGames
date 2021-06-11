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

	// static, skinned 두 가지로 구분한다. => 언리얼 모델이다.
	// renderable에서 buffer를 참조하기 위해 이 객체를 참조한다.
	// 현재 Resource는 아무것도 없다. 추후 공통 사항을 뽑아내자.
	// => 여전히 복사를 해결하지 못했다. 그리고 Instancing 적용 여부도 생각해야 한다.
	class dvMesh : public Resource
	{
	public:
		dvMesh();
		~dvMesh();

		bool SaveToFile(const std::string& filepath);			// Engine Format으로 저장한다.
		bool LoadFromFile(const std::string& filepath);			// Engine Format만 로드한다. 이 과정에서 GameObject도 생성해야 한다.

		bool CreateGeometryBuffer();

		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }

		const eMeshType& GetMeshType() const { return m_type; }
		unsigned int GetVertexCount() const { return static_cast<unsigned int>(m_positions.size()); }
		unsigned int GetIndexCount() const { return static_cast<unsigned int>(m_indices.size()); }

	private:
		// 버퍼를 생성할 때 빈 요소들을 직접 계산해야 한다.

	public:
		std::vector<DirectX::XMFLOAT3> m_positions;
		std::vector<DirectX::XMFLOAT2> m_uv;
		std::vector<DirectX::XMFLOAT2> m_uv1;
		std::vector<DirectX::XMFLOAT4> m_colors;
		std::vector<DirectX::XMFLOAT3> m_normals;
		std::vector<DirectX::XMFLOAT3> m_tangents;
		// bone 여부를 통해 static과 skinned가 구분된다.
		std::vector<unsigned int> m_indices;

	private:
		GameObject* m_pRoot;	// 꼭 필요하다.

		std::string m_name;
		eMeshType m_type;

		// 다 묶어서 Buffer class를 만들까...
		unsigned int m_stride;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
	};
}