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
		m_gpuBuffer = std::make_unique<ConstantBuffer>(
			eVSConstantBufferSlot::Object, 
			static_cast<uint32_t>(sizeof(ObjectData)));
		if (!m_gpuBuffer) DV_LOG(MeshRenderer, err, "[::MeshRenderer] ConstantBuffer 생성 실패");
	}

	MeshRenderer::~MeshRenderer()
	{
		m_gpuBuffer.reset();
	}

	void MeshRenderer::Render(const Camera* camera)
	{
		assert(camera);
		
		ObjectData objectData{};
		objectData.model = XMMatrixTranspose(GetGameObject()->GetTransform()->GetTransformMatrix());
		objectData.View = XMMatrixTranspose(camera->GetViewMatrix());
		objectData.Proj = XMMatrixTranspose(camera->GetProjectionMatrix());
		m_gpuBuffer->Update<ObjectData>(objectData);
		m_gpuBuffer->Bind();

		m_material->Bind();

		m_staticMesh->Draw();
	}

	bool MeshRenderer::IsVisible(const Frustum& frustum)
	{
		assert(m_staticMesh);
		const auto boundingBox = m_staticMesh->GetBoundingBox();
		return frustum.CheckAABB(boundingBox->GetCenter(), boundingBox->GetExtent());
	}
	
	bool MeshRenderer::IsTransparent() const
	{
		assert(m_material);
		return m_material->IsTransparent();
	}
}
