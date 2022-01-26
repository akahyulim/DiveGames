#include "divepch.h"
#include "MeshRenderer.h"
#include "../GameObject.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Graphics/GraphicsDevice.h"
#include "Base/Log.h"
#include "Utils/Geometry.h"
#include "Base/FileStream.h"
#include "../../Renderer/Material.h"

namespace Dive
{
	MeshRenderer::MeshRenderer(GameObject* pGameObject)
		: Component(pGameObject)
	{
		m_Type = eComponentType::MeshRenderer;

		// �̰� �ٸ� ���� ������ �ʿ䰡 ���ٸ�
		// unique_ptr�� �� ��︰��.
		m_pMesh = new Mesh;

		REGISTER_ATTRIBUTE_VALUE_VALUE(m_pMesh, Mesh*);
	}

	MeshRenderer::~MeshRenderer()
	{
		if (m_pMesh)
		{
			delete m_pMesh;
			m_pMesh = nullptr;
		}
	}

	// �ϴ� ������ �͸� ����.
	void MeshRenderer::Serialize(FileStream* pFileStream)
	{
		if (!pFileStream)
		{
			CORE_ERROR("");
			return;
		}

		// basic
		{
			pFileStream->Write((int)m_MeshType);
		}
	}

	void MeshRenderer::Deserialize(FileStream* pFileStream)
	{
		if (!pFileStream)
		{
			CORE_ERROR("");
			return;
		}

		{
			pFileStream->Read((int*)&m_MeshType);
		}

		if (m_MeshType != eDefaultMeshType::None)
		{
			CreateDefaultMesh(m_MeshType);
		}
	}

	// ���� Mesh�� Resource�� ������� �ʾҴ�.
	// �׷��� �ߺ����� �Ǵ� ������ �ִ�.
	// ���ĸ�ź�� ��� Resource�� ����ϸ�
	// �� �Լ��� �ش��ϴ� ������ ����ȭ�� ���� filepath �������� �� ���´�.
	// �׷����� �ұ��ϰ� �ߺ� ���� ������ ���� �� ����.
	// ������ ���� ���ϸ� ������ ������ �߻��� �� �ִ�.
	// ��¼�� GameObject ���� ����� ���� �ذ��� ���ϼ���...?
	// ������ Entity::Clone()���� Component�� Attributes �Լ��� �� �־� ���δ�...
	void MeshRenderer::CreateDefaultMesh(eDefaultMeshType type)
	{
		m_MeshType = type;

		// �Լ��� ����� �ʹ�.
		if (!m_pMesh->GetVertices().empty() || !m_pMesh->GetIndices().empty())
		{
			m_pMesh->Clear();
		}
		if (m_pVertexBuffer)
		{
			m_pVertexBuffer->Release();
			m_pVertexBuffer = nullptr;
		}
		if (m_pIndexBuffer)
		{
			m_pIndexBuffer->Release();
			m_pIndexBuffer = nullptr;
		}

		auto& vertices = m_pMesh->GetVertices();
		auto& indices = m_pMesh->GetIndices();

		switch (type)
		{
		case eDefaultMeshType::Quad:
		{
			Geometry::CreatePlane(vertices, indices);
			m_pMesh->SetName("Quad");
			break;
		}
		case eDefaultMeshType::Cube:
		{
			Geometry::CreateCube(vertices, indices);
			m_pMesh->SetName("Cube");
			break;
		}
		default:
			return;
		}

		assert(!vertices.empty());
		assert(!indices.empty());

		if (!createBuffers())
		{
			delete m_pMesh;
			m_pMesh = nullptr;
		}
	}

	// Draw�� �� ��︱ �� ����.
	void MeshRenderer::Render(ID3D11DeviceContext* pDeviceContext)
	{
		if (!pDeviceContext)
			return;

		// ���Ⱑ �³� �𸣰ڴ�.
		if (!GetGameObject()->IsActive())
			return;


		// Mesh�� Buffer�� �����ٸ� �̷��� ���°� �´�.
		// �ٸ� 2�� �̻��� ������ �����ٸ� Offset�� GameObject(Transform)�� ��Ī�� �ʿ��ϴ�.
		auto pVertexBuffer = m_pMesh->GetVertexBuffer();
		auto vertexStride = m_pMesh->GetVertexStride();
		auto vertexOffset = m_pMesh->GetVertexBufferOffset();
		pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &vertexStride, &vertexOffset);

		auto pIndexBuffer = m_pMesh->GetIndexBuffer();
		auto indexOffset = m_pMesh->GetIndexBufferOffset();
		pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, indexOffset);

		pDeviceContext->DrawIndexed(m_pMesh->GetIndexCount(), 0, 0);
	}

	bool MeshRenderer::createBuffers()
	{
		auto pDevice = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(pDevice);

		return m_pMesh->CreateGeometryBuffers(pDevice);
	}
}