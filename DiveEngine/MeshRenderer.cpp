#include "MeshRenderer.h"
#include "Renderer.h"
#include "GraphicsDevice.h"
#include "Log.h"

namespace dive
{
	MeshRenderer::MeshRenderer()
		: Component(typeid(MeshRenderer).hash_code())
	{
	}

	MeshRenderer::~MeshRenderer()
	{
	}
	
	void MeshRenderer::SetMesh(const dvMesh& mesh)
	{
		mMesh = const_cast<dvMesh*>(&mesh);

		calcu();

		if (!createBuffers())
		{

		}
	}

	void MeshRenderer::calcu()
	{
		// 빈 데이터를 계산한 후 추가한다.
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