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
		// 이걸 다른 곳에 전달할 필요가 없다면
		// unique_ptr에 딱 어울린다.
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
		// 값을 저장하기 보단
		// 어떤 Mesh를 만들었느냐를 저장해야 한다.
		// 즉, Default일 경우 어떤 타입이었냐
		// 파일일 경우 경로를 저장해야 한다.

		// 추후 Material과 기타 설정값들도 추가해야 한다.
	}

	void MeshRenderer::Deserialize(FileStream* fileStream)
	{
		// 저장된 타입 혹은 파일 경로를 읽어 들인 후
		// Mesh를 구성하고 버퍼를 생성해야 한다.
	}

	// 모델 로드일 땐 다른 구현이 필요하다.
	// 아마도 LoadFromFile이 되겠지?
	void MeshRenderer::CreateDefaultMesh(eDefaultMeshType type)
	{
		// 함수로 만들까 싶다.
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

	void MeshRenderer::Render(ID3D11DeviceContext* deviceContext)
	{
		if (!deviceContext)
			return;

		// ref에선 constant buffer와 input layout, shader를 외부에서 먼저 bind 시켰다.
		// 그렇다면 이 곳에선 Material과 기타 데이터를 bind해야 하는데
		// 일단은 buffer만 bind한 후 draw 하자.
		// 문제는 추후 Model을 어떻게 그려야하느냐 이다.

		unsigned int offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &mStride, &offset);
		deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		deviceContext->DrawIndexed(GetIndexCount(), 0, 0);
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
			// 일단 그냥 막 코딩
			// VertexStride만 VertexBuffer Bind에 사용되기 때문이다.
			stride = static_cast<unsigned int>(sizeof(VertexType_PosTexNorTan));
			mStride = stride;

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