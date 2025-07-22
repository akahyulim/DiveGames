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
		m_ObjectBuffer = std::make_unique<DvConstantBuffer>(Graphics::GetDevice(), eVSConstantBufferSlot::Object, sizeof(ObjectData));

		DV_LOG(MeshRenderer, info, "생성: {}, {}", GetName(), GetInstanceID());
	}

	MeshRenderer::~MeshRenderer()
	{
		DV_LOG(MeshRenderer, info, "소멸: {}, {}", GetName(), GetInstanceID());
	}

	void MeshRenderer::Render(ID3D11DeviceContext* deviceContext, const Camera* camera)
	{
		assert(deviceContext);
		assert(camera);
		
		ObjectData objectData{};
		objectData.model = XMMatrixTranspose(GetGameObject()->GetTransform()->GetTransformMatrix());
		objectData.View = XMMatrixTranspose(camera->GetViewMatrix());
		objectData.Proj = XMMatrixTranspose(camera->GetProjectionMatrix());
		m_ObjectBuffer->Update<ObjectData>(deviceContext, objectData);
		m_ObjectBuffer->Bind(deviceContext);	// 굳이 update와 나눌 필요가 있나?

		m_Material->Bind(deviceContext);

		m_StaticMesh->Draw(deviceContext);
	}

	bool MeshRenderer::IsVisible(const Frustum& frustum)
	{
		assert(m_StaticMesh);
		const auto boundingBox = m_StaticMesh->GetBoundingBox();
		return frustum.CheckAABB(boundingBox->GetCenter(), boundingBox->GetExtent());
	}
	
	bool MeshRenderer::IsTransparent() const
	{
		assert(m_Material);
		return m_Material->IsTransparent();
	}
}
