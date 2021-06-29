#include "MeshRenderer.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "Log.h"
#include "Geometry.h"

namespace dive
{
	MeshRenderer::MeshRenderer()
		: Component(typeid(MeshRenderer).hash_code())
	{
		// �̰� �ٸ� ���� ������ �ʿ䰡 ���ٸ�
		// unique_ptr�� �� ��︰��.
		mMesh = new Mesh;
	}

	MeshRenderer::~MeshRenderer()
	{
		if (mMesh)
		{
			delete mMesh;
			mMesh = nullptr;
		}
	}

	void MeshRenderer::Serialize(FileStream* fileStream)
	{
		// ���� �����ϱ� ����
		// � Mesh�� ��������ĸ� �����ؾ� �Ѵ�.
		// ��, Default�� ��� � Ÿ���̾���
		// ������ ��� ��θ� �����ؾ� �Ѵ�.

		// ���� Material�� ��Ÿ �������鵵 �߰��ؾ� �Ѵ�.
	}

	void MeshRenderer::Deserialize(FileStream* fileStream)
	{
		// ����� Ÿ�� Ȥ�� ���� ��θ� �о� ���� ��
		// Mesh�� �����ϰ� ���۸� �����ؾ� �Ѵ�.
	}

	// �� �ε��� �� �ٸ� ������ �ʿ��ϴ�.
	// �Ƹ��� LoadFromFile�� �ǰ���?
	void MeshRenderer::CreateDefaultMesh(eDefaultMeshType type)
	{
		// �Լ��� ����� �ʹ�.
		if (!mMesh->GetVertices().empty() || !mMesh->GetIndices().empty())
		{
			mMesh->Clear();
		}
		if (mVertexBuffer)
		{
			mVertexBuffer->Release();
			mVertexBuffer = nullptr;
		}
		if (mIndexBuffer)
		{
			mIndexBuffer->Release();
			mIndexBuffer = nullptr;
		}

		auto& vertices = mMesh->GetVertices();
		auto& indices = mMesh->GetIndices();

		switch (type)
		{
		case eDefaultMeshType::Triangle:
		{
			break;
		}
		case eDefaultMeshType::Quad:
		{
			utility::geometry::CreateQuad(vertices, indices);
			mMesh->SetName("Quad");
			break;
		}
		case eDefaultMeshType::Cube:
		{
			utility::geometry::CreateCube(vertices, indices);
			mMesh->SetName("Cube");
			break;
		}
		default:
			return;
		}

		assert(!vertices.empty());
		assert(!indices.empty());

		if (!createBuffers())
		{
			delete mMesh;
			mMesh = nullptr;
		}
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
			stride = static_cast<unsigned int>(sizeof(VertexType_PosTexNorTan));

			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;
			desc.ByteWidth = stride * mMesh->GetVertexCount();
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = mMesh->GetVertices().data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			if (FAILED(device->CreateBuffer(&desc, &data, &mVertexBuffer)))
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
			desc.ByteWidth = stride * mMesh->GetIndexCount();
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = mMesh->GetIndices().data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			if (FAILED(device->CreateBuffer(&desc, &data, &mIndexBuffer)))
			{
				return false;
			}
		}

		return true;
	}
}