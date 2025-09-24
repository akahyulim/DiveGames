#pragma once
#include "Component.h"
#include "math/Frustum.h"
#include "math/BoundingBox.h"

namespace Dive
{
	class StaticMesh;
	class Material;
	class ConstantBuffer;
	class Camera;

	struct alignas(16) ObjectData
	{
		DirectX::XMFLOAT4X4 worldMatrix;
	};

	class MeshRenderer : public Component
	{
	public:
		MeshRenderer(GameObject* gameObject);
		~MeshRenderer() override;

		void Update() override;

		virtual void Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);
		bool IsVisible(const Frustum& frustum);

		std::shared_ptr<StaticMesh> GetStaticMesh() const { return m_StaticMesh; }
		void SetStaticMesh(std::shared_ptr<StaticMesh> staticMesh);

		std::shared_ptr<Material> GetMaterial() const { return m_Material; }
		void SetMaterial(std::shared_ptr<Material> material) { m_Material = material; }

		const BoundingBox& GetBoundingBox() const { return m_BoundingBox; }

		static constexpr eComponentType GetComponentType() { return eComponentType::MeshRenderer; }

	private:
		std::shared_ptr<StaticMesh> m_StaticMesh;
		std::shared_ptr<Material> m_Material;

		std::unique_ptr<ConstantBuffer> m_cbObjectVS;

		BoundingBox m_BoundingBox;
	};
}
