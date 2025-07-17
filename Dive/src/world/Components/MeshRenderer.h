#pragma once
#include "Component.h"
#include "math/Frustum.h"

namespace Dive
{
	class StaticMesh;
	class Material;
	class DvConstantBuffer;
	class Camera;

	struct ObjectData
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

		virtual void Render(ID3D11DeviceContext* deviceContext, const Camera* camera);
		bool IsVisible(const Frustum& frustum);
		bool IsTransparent() const;

		std::shared_ptr<StaticMesh> GetStaticMesh() const { return m_StaticMesh; }
		void SetStaticMesh(std::shared_ptr<StaticMesh> staticMesh) { m_StaticMesh = staticMesh; }

		std::shared_ptr<Material> GetMaterial() const { return m_Material; }
		void SetMaterial(std::shared_ptr<Material> material) { m_Material = material; }

		static constexpr eComponentType GetType() { return eComponentType::MeshRenderer; }

	private:
		std::shared_ptr<StaticMesh> m_StaticMesh;
		std::shared_ptr<Material> m_Material;

		std::unique_ptr<DvConstantBuffer> m_ObjectBuffer;
	};
}
