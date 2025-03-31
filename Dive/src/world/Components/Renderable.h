#pragma once
#include "Component.h"
#include "Math/BoundingBox.h"
//#include "Renderer/Renderer.h"

namespace Dive
{
	class GameObejct;
	class Material;
	class Mesh;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;

	class Renderable : public Component
	{
		DV_CLASS(Renderable, Component)

	public:
		Renderable(GameObject* pGameObject);
		~Renderable() override;

		void Update() override;

		ConstantBuffer* GetConstantBufferVS() override;// { return m_pCBufferVS; }
		ConstantBuffer* GetConstantBufferPS() override;// { return m_pCBufferPS; }

		void SetGeometry(Mesh* pMesh, uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount);

		bool HasMaterial() const { return m_pMaterial; }
		Material* GetMaterial() const { return m_pMaterial; }
		void SetMaterial(Material* pMaterial) { m_pMaterial = pMaterial; }

		Mesh* GetMesh() { return m_pMesh; }

		VertexBuffer* GetVertexBuffer() const;
		uint32_t GetVertexOffset() const { return m_VertexOffset; }
		uint32_t GetVertexCount() const { return m_VertexCount; }

		IndexBuffer* GetIndexBuffer() const;
		uint32_t GetIndexOffset() const { return m_IndexOffset; }
		uint32_t GetIndexCount() const { return m_IndexCount; }

		const BoundingBox& GetBoundingBox();

		void Draw(D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) const;

	private:

	private:
		Mesh* m_pMesh;
		Material* m_pMaterial;

		uint32_t m_VertexOffset;
		uint32_t m_VertexCount;
		uint32_t m_IndexOffset;
		uint32_t m_IndexCount;

		BoundingBox m_BoundingBox;
		BoundingBox m_TransformdBoundingBox;

		DirectX::XMFLOAT4X4 m_LastTransform;

		ConstantBuffer* m_pCBufferVS;
		ConstantBuffer* m_pCBufferPS;
	};
}