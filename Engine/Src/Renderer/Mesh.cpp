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
			DV_CORE_ERROR("StaticMesh::CreateVertexBuffer: VertexBuffer ������ �����Ͽ����ϴ�.");
			return;
		}
	}

	void StaticMesh::CreateIndexBuffer()
	{
		m_pIndexBuffer = new IndexBuffer;
		if (!m_pIndexBuffer->Create<uint32_t>(m_Indices))
		{
			DV_CORE_ERROR("StaticMesh::CreateIndexBuffer: IndexBuffer ������ �����Ͽ����ϴ�.");
			return;
		}
	}

	void StaticMesh::Render(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)
	{
		// �� �κ��� ���ٴ�.
		// ����Ű�� Scene::Render()���� �ش� ����� ȹ���� �� Renderer(RenderPath)�� SubmitMesh�� �����Ͽ�
		// �ý��� ��� ���۷� ���ε� �ǵ��� �ߴ�. 
		//auto pBuffer = Renderer::GetModelVertexShaderBuffer();
		//auto pMappedData = static_cast<ModelVertexShaderBuffer*>(pBuffer->Map());
		//pMappedData->worldMatrix = DirectX::XMMatrixTranspose(m_pGameObject->GetTransform()->GetWorldMatrix());
		//pBuffer->Unmap();
		//Graphics::SetConstantBuffer(1, eShaderType::VertexShader, pBuffer);

		// �� �κ��� �ٽ� Vertex, Index Buffer�� Bind�� Draw�� ȣ���Ѵ�.
		// �̸� ���ؼ� ���� ������ ��� ��ǲ ���̾ƿ��� ������ �Ѵ�.
		//Graphics::SetVertexBuffer(m_pMesh->GetVertexBuffer());
		//Graphics::SetIndexBuffer(m_pMesh->GetIndexBuffer());
		//Graphics::DrawIndexed(
		//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		//	m_pMesh->GetNumIndices(),
		//	0, 0);
	}
};

