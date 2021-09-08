#include "MeshRenderer.h"
#include "GameObject.h"
#include "Src/Renderer/Renderer.h"
#include "Src/Renderer/Graphics/GraphicsDevice.h"
#include "Log.h"
#include "Geometry.h"
#include "FileStream.h"

namespace dive
{
	MeshRenderer::MeshRenderer(GameObject* pGameObject)
		: Component(typeid(MeshRenderer).hash_code(), pGameObject)
	{
		m_Type = eComponentType::MeshRenderer;

		// 이걸 다른 곳에 전달할 필요가 없다면
		// unique_ptr에 딱 어울린다.
		m_pMesh = new Mesh;
	}

	MeshRenderer::~MeshRenderer()
	{
		if (m_pMesh)
		{
			delete m_pMesh;
			m_pMesh = nullptr;
		}
	}

	// 일단 구현한 것만 넣자.
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

	// 모델 로드일 땐 다른 구현이 필요하다.
	// 아마도 LoadFromFile이 되겠지?
	// 이렇게 하면 결국 Model은 중복생성된다.
	// 게다가 버퍼까지 MeshRenderer마다 개별로 생성된다.
	// 그런데 프리팹을 만들거라면 좀 더 생각해봐야 한다.
	// 유니티의 경우 이와같은 기본 Geometry는 프리팹으로 생성하지 않는다.
	void MeshRenderer::CreateDefaultMesh(eDefaultMeshType type)
	{
		m_MeshType = type;

		// 함수로 만들까 싶다.
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
		case eDefaultMeshType::Triangle:
		{
			break;
		}
		case eDefaultMeshType::Quad:
		{
			utility::geometry::CreateQuad(vertices, indices);
			m_pMesh->SetName("Quad");
			break;
		}
		case eDefaultMeshType::Cube:
		{
			utility::geometry::CreateCube(vertices, indices);
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

	void MeshRenderer::Render(ID3D11DeviceContext* deviceContext)
	{
		if (!deviceContext)
			return;

		// 여기가 맞나 모르겠다.
		if (!GetGameObject()->IsActive())
			return;

		// ref에선 constant buffer와 input layout, shader를 외부에서 먼저 bind 시켰다.
		// 그렇다면 이 곳에선 Material과 기타 데이터를 bind해야 하는데
		// 일단은 buffer만 bind한 후 draw 하자.
		// 문제는 추후 Model을 어떻게 그려야하느냐 이다.

		unsigned int offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_Stride, &offset);
		deviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

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
			m_Stride = stride;

			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;
			desc.ByteWidth = stride * m_pMesh->GetVertexCount();
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = m_pMesh->GetVertices().data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			if (FAILED(device->CreateBuffer(&desc, &data, &m_pVertexBuffer)))
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
			desc.ByteWidth = stride * m_pMesh->GetIndexCount();
			desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = m_pMesh->GetIndices().data();
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			if (FAILED(device->CreateBuffer(&desc, &data, &m_pIndexBuffer)))
			{
				return false;
			}
		}

		return true;
	}
}