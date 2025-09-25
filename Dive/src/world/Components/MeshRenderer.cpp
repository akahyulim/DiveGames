#include "stdafx.h"
#include "MeshRenderer.h"
#include "graphics/Shader.h"
#include "graphics/ConstantBuffer.h"
#include "rendering/RenderDefs.h"
#include "rendering/StaticMesh.h"
#include "rendering/Material.h"
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

	void MeshRenderer::Update()
	{
		// 일단 이걸 무조건 통과한다.
		// 컴포넌트의 생성순서 때문인 듯하다.
		if (!GetTransform()->WasUpdated())
			return;

		m_BoundingBox = m_StaticMesh->GetLocalBoundingBox().Transform(GetTransform()->GetTransformMatrix());
	}

	void MeshRenderer::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
	{
		ObjectData objectData{};
		XMStoreFloat4x4(&objectData.worldMatrix, XMMatrixTranspose(GetTransform()->GetTransformMatrix()));
		m_cbObjectVS->Update<ObjectData>(objectData);
		m_cbObjectVS->BindVS(eCBufferSlotVS::Object);

		m_Material->Bind();

		m_StaticMesh->Draw();
	}

	bool MeshRenderer::IsVisible(const Frustum& frustum) const
	{
		return frustum.CheckAABB(m_BoundingBox.GetCenter(), m_BoundingBox.GetExtent());
	}

	bool MeshRenderer::Intersects(const Ray& ray, float* outDistance, DirectX::XMFLOAT3* outPoint, DirectX::XMFLOAT3* outNormal) const
	{
		if (!m_BoundingBox.Intersects(ray, outDistance, outNormal))
			return false;

		*outPoint = {
			ray.origin.x + ray.direction.x * (*outDistance), 
			ray.origin.y + ray.direction.y * (*outDistance), 
			ray.origin.z + ray.direction.z * (*outDistance)
		};

		return true;
	}

	void MeshRenderer::SetStaticMesh(std::shared_ptr<StaticMesh> staticMesh)
	{
		m_StaticMesh = staticMesh;
		m_BoundingBox = staticMesh->GetLocalBoundingBox();
	}
}
