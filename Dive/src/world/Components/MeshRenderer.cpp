#include "stdafx.h"
#include "MeshRenderer.h"
#include "graphics/Shader.h"
#include "graphics/ConstantBuffer.h"
#include "rendering/RenderDefs.h"
#include "rendering/StaticMesh.h"
#include "rendering/Material.h"
#include "math/BoundingBox.h"
#include "world/GameObject.h"
#include "world/Components/Camera.h"
#include "world/Components/Transform.h"

using namespace DirectX;

namespace Dive
{
	MeshRenderer::MeshRenderer(GameObject* gameObject)
		: Component(gameObject)
	{
		m_cbObjectVS = std::make_unique<ConstantBuffer>(static_cast<uint32_t>(sizeof(ObjectData)));
		if (!m_cbObjectVS) DV_LOG(MeshRenderer, err, "[::MeshRenderer] cbObjectVS 생성 실패");
	}

	MeshRenderer::~MeshRenderer()
	{
		m_cbObjectVS.reset();
	}

	void MeshRenderer::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
	{
		ObjectData objectData{};
		objectData.worldMatrix = XMMatrixTranspose(GetGameObject()->GetTransform()->GetTransformMatrix());
		m_cbObjectVS->Update<ObjectData>(objectData);
		m_cbObjectVS->BindVS(eCBufferSlotVS::Object);

		m_material->Bind();

		m_staticMesh->Draw();
	}

	bool MeshRenderer::IsVisible(const Frustum& frustum)
	{
		assert(m_staticMesh);
		const auto boundingBox = m_staticMesh->GetBoundingBox();
		return frustum.CheckAABB(boundingBox->GetCenter(), boundingBox->GetExtent());
	}
}
