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
	// 여기도 Device를 전달받는 편이 어울릴 것 같다.
	MeshRenderer::MeshRenderer(GameObject* gameObject)
		: Component(gameObject)
	{
		m_gpuBuffer = std::make_unique<ConstantBuffer>(
			Graphics::GetDevice(), 
			eVSConstantBufferSlot::Object, 
			static_cast<uint32_t>(sizeof(ObjectData)));
		if (!m_gpuBuffer) DV_LOG(MeshRenderer, err, "[::MeshRenderer] ConstantBuffer 생성 실패");

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
		m_gpuBuffer->Update<ObjectData>(deviceContext, objectData);
		m_gpuBuffer->Bind(deviceContext);	// 굳이 update와 나눌 필요가 있나?

		m_material->Bind(deviceContext);

		m_staticMesh->Draw(deviceContext);
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
