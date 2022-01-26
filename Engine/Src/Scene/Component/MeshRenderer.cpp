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

		// 이걸 다른 곳에 전달할 필요가 없다면
		// unique_ptr에 딱 어울린다.
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

	// 현재 Mesh가 Resource를 상속하지 않았다.
	// 그래서 중복생성 되는 문제가 있다.
	// 스파르탄의 경우 Resource를 상속하며
	// 이 함수에 해당하는 곳에서 파일화를 위한 filepath 설정까지 해 놓는다.
	// 그럼에도 불구하고 중복 생성 문제는 피할 수 없다.
	// 오히려 동일 파일명 때문에 문제가 발생할 수 있다.
	// 어쩌면 GameObject 복사 방법을 통해 해결한 것일수도...?
	// 실제로 Entity::Clone()에서 Component의 Attributes 함수가 좀 있어 보인다...
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

	// Draw가 더 어울릴 것 같다.
	void MeshRenderer::Render(ID3D11DeviceContext* pDeviceContext)
	{
		if (!pDeviceContext)
			return;

		// 여기가 맞나 모르겠다.
		if (!GetGameObject()->IsActive())
			return;


		// Mesh가 Buffer를 가진다면 이러한 형태가 맞다.
		// 다만 2개 이상의 부위로 나뉜다면 Offset과 GameObject(Transform)의 매칭이 필요하다.
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