#pragma once
#include "Component.h"
#include "math/Frustum.h"

namespace Dive
{
	class StaticMesh;
	class Material;
	class ConstantBuffer;
	class Camera;

	struct alignas(16) ObjectData
	{
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Proj;
	};

	class MeshRenderer : public Component
	{
	public:
		MeshRenderer(GameObject* gameObject);
		~MeshRenderer() override;

		virtual void Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);
		bool IsVisible(const Frustum& frustum);

		std::shared_ptr<StaticMesh> GetStaticMesh() const { return m_staticMesh; }
		void SetStaticMesh(std::shared_ptr<StaticMesh> staticMesh) { m_staticMesh = staticMesh; }

		std::shared_ptr<Material> GetMaterial() const { return m_material; }
		void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }

		static constexpr eComponentType GetComponentType() { return eComponentType::MeshRenderer; }

	private:
		std::shared_ptr<StaticMesh> m_staticMesh;
		std::shared_ptr<Material> m_material;

		std::unique_ptr<ConstantBuffer> m_gpuBuffer;
	};
}
