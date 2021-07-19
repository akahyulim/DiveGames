#include "MeshRenderer.h"
#include "GameObject.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "Log.h"
#include "Geometry.h"
#include "FileStream.h"

namespace dive
{
	MeshRenderer::MeshRenderer(GameObject* gameObject)
		: Component(typeid(MeshRenderer).hash_code(), gameObject)
	{
		// �̰� �ٸ� ���� ������ �ʿ䰡 ���ٸ�
		// unique_ptr�� �� ��︰��.
		m_Mesh = new Mesh;
	}

	MeshRenderer::~MeshRenderer()
	{
		if (m_Mesh)
		{
			delete m_Mesh;
			m_Mesh = nullptr;
		}
	}

	// �ϴ� ������ �͸� ����.
	void MeshRenderer::Serialize(FileStream* fileStream)
	{
		if (!fileStream)
		{
			CORE_ERROR("");
			return;
		}

		// basic
		{
			fileStream->Write((int)m_MeshType);
		}
	}

	void MeshRenderer::Deserialize(FileStream* fileStream)
	{
		if (!fileStream)
		{
			CORE_ERROR("");
			return;
		}

		{
			fileStream->Read((int*)&m_MeshType);
		}

		if (m_MeshType != eDefaultMeshType::None)
		{
			CreateDefaultMesh(m_MeshType);
		}
	}

	// �� �ε��� �� �ٸ� ������ �ʿ��ϴ�.
	// �Ƹ��� LoadFromFile�� �ǰ���?
	// �̷��� �ϸ� �ᱹ Model�� �ߺ������ȴ�.
	// �Դٰ� ���۱��� MeshRenderer���� ������ �����ȴ�.
	// �׷��� �������� ����Ŷ�� �� �� �����غ��� �Ѵ�.
	// ����Ƽ�� ��� �̿Ͱ��� �⺻ Geometry�� ���������� �������� �ʴ´�.
	void MeshRenderer::CreateDefaultMesh(eDefaultMeshType type)
	{
		m_MeshType = type;

		// �Լ��� ����� �ʹ�.
		if (!m_Mesh->GetVertices().empty() || !m_Mesh->GetIndices().empty())
		{
			m_Mesh->Clear();
		}
		if (m_VertexBuffer)
		{
			m_VertexBuffer->Release();
			m_VertexBuffer = nullptr;
		}
		if (m_IndexBuffer)
		{
			m_IndexBuffer->Release();
			m_IndexBuffer = nullptr;
		}

		auto& vertices = m_Mesh->GetVertices();
		auto& indices = m_Mesh->GetIndices();

		switch (type)
		{
		case eDefaultMeshType::Triangle:
		{
			break;
		}
		case eDefaultMeshType::Quad:
		{
			utility::geometry::CreateQuad(vertices, indices);
			m_Mesh->SetName("Quad");
			break;
		}
		case eDefaultMeshType::Cube:
		{
			utility::geometry::CreateCube(vertices, indices);
			m_Mesh->SetName("Cube");
			break;
		}
		default:
			return;
		}

		assert(!vertices.empty());
		assert(!indices.empty());

		if (!createBuffers())
		{
			delete m_Mesh;
			m_Mesh = nullptr;
		}
	}

	void MeshRenderer::Render(ID3D11DeviceContext* deviceContext)
	{
		if (!deviceContext)
			return;

		// ���Ⱑ �³� �𸣰ڴ�.
		if (!GetGameObject()->IsActive())
			return;

		// ref���� constant buffer�� input layout, shader�� �ܺο��� ���� bind ���״�.
		// �׷��ٸ� �� ������ Material�� ��Ÿ �����͸� bind�ؾ� �ϴµ�
		// �ϴ��� buffer�� bind�� �� draw ����.
		// ������ ���� Model�� ��� �׷����ϴ��� �̴�.

		unsigned int offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &m_Stride, &offset);
		deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		deviceContext->DrawIndexed(GetIndexCount(), 0, 0);
	}

	void MeshRenderer::calcu()
	{
		// �� �����͸� ����� �� �߰��Ѵ�.
	}
	
	bool MeshRenderer::createBuffers()
	{
		auto device = Renderer::GetInstance().GetGraphicsDevice()->GetDevice();
		assert(device);

		unsigned int stride = 0;

		// vertex buffer
		{
			// �ϴ� �׳� �� �ڵ�
			// VertexStride�� VertexBuffer Bind�� ���Ǳ� �����̴�.
			stride = static_cast<unsigned int>(sizeof(VertexType_PosTexNorTan));
			m_Stride = stride;

			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;
			desc.ByteWidth = stride * m_Mesh->GetVertexCount();
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = m_Mesh->GetVertices().data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			if (FAILED(device->CreateBuffer(&desc, &data, &m_VertexBuffer)))
			{
				return false;
			}
		}

		// index buffer
		{
			stride = static_cast<unsigned int>(sizeof(unsigned int));

			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;
			desc.ByteWidth = stride * m_Mesh->GetIndexCount();
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = m_Mesh->GetIndices().data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			if (FAILED(device->CreateBuffer(&desc, &data, &m_IndexBuffer)))
			{
				return false;
			}
		}

		return true;
	}
}