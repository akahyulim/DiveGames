#include "DivePch.h"
#include "Mesh.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"

namespace Dive
{
	Mesh::Mesh()
		: m_pVertexBuffer(nullptr),
		m_pIndexBuffer(nullptr)
	{
	}

	Mesh::~Mesh()
	{
		DV_DELETE(m_pVertexBuffer);
		DV_DELETE(m_pIndexBuffer);
	}

	StaticMesh::StaticMesh(const std::string& name, std::vector<VertexStatic>& vertices, std::vector<uint32_t>& indices)
		: m_Vertices(vertices),
		m_Indices(indices)
	{
		m_Name = name;
	}

	StaticMesh::StaticMesh(const std::string& nodeName, const std::string& name, std::vector<VertexStatic>& vertices, std::vector<uint32_t>& indices)
		: m_Vertices(vertices),
		m_Indices(indices)
	{
		m_NodeName = nodeName;
		m_Name = name.empty() ? nodeName : name;
	}

	StaticMesh::~StaticMesh()
	{

	}

	void StaticMesh::CreateVertexBuffer()
	{
		m_pVertexBuffer = new VertexBuffer;
		if (!m_pVertexBuffer->Create<VertexStatic>(m_Vertices))
		{
			DV_CORE_ERROR("StaticMesh::CreateVertexBuffer: VertexBuffer 생성에 실패하였습니다.");
			return;
		}
	}

	void StaticMesh::CreateIndexBuffer()
	{
		m_pIndexBuffer = new IndexBuffer;
		if (!m_pIndexBuffer->Create<uint32_t>(m_Indices))
		{
			DV_CORE_ERROR("StaticMesh::CreateIndexBuffer: IndexBuffer 생성에 실패하였습니다.");
			return;
		}
	}

	void StaticMesh::Render(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
	{
		// 이 부분은 에바다.
		// 스파키는 Scene::Render()에서 해당 행렬을 획득한 후 Renderer(RenderPath)의 SubmitMesh에 전달하여
		// 시스템 상수 버퍼로 바인드 되도록 했다. 
		//auto pBuffer = Renderer::GetModelVertexShaderBuffer();
		//auto pMappedData = static_cast<ModelVertexShaderBuffer*>(pBuffer->Map());
		//pMappedData->worldMatrix = DirectX::XMMatrixTranspose(m_pGameObject->GetTransform()->GetWorldMatrix());
		//pBuffer->Unmap();
		//Graphics::SetConstantBuffer(1, eShaderType::VertexShader, pBuffer);

		// 이 부분은 다시 Vertex, Index Buffer의 Bind와 Draw를 호출한다.
		// 이를 위해선 정점 버퍼의 경우 인풋 레이아웃도 가져야 한다.
		//Graphics::SetVertexBuffer(m_pMesh->GetVertexBuffer());
		//Graphics::SetIndexBuffer(m_pMesh->GetIndexBuffer());
		//Graphics::DrawIndexed(
		//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		//	m_pMesh->GetNumIndices(),
		//	0, 0);
	}
};

