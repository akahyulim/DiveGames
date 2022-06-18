#pragma once
#include "Renderable.h"

namespace Dive
{
	class GameObject;
	class Model;

	// Import�� �� �ش� GameObject�� ������
	// name, offset, count, model�� �޾� �����Ѵ�.
	// ���� draw�� �� ����Ѵ�.
	// SpriteRenderable�� interface�� ����������.
	class MeshRenderable : public Renderable
	{
		COMPONENT_CLASS_TYPE(MeshRenderable)

	public:
		MeshRenderable(GameObject* pGameObject);
		~MeshRenderable();

		// ���� bounding box����
		void SetGeometry(const std::string& name, unsigned int vertexOffset, unsigned int vertexCount,
			unsigned int indexOffset, unsigned int indexCount, Model* pModel);

		std::string GetGeometryName() const { return m_GeometryName; }

		unsigned int VertexOffset() const { return m_VertexOffset; }
		unsigned int VertexCount() const { return m_VertexCount; }

		unsigned int IndexOffset() const { return m_IndexOffset; }
		unsigned int IndexCount() const { return m_IndexCount; }

		Model* GetModel() const { return m_pModel; }

	private:

	private:
		std::string m_GeometryName	= "Unknown";
		unsigned int m_VertexOffset = 0;
		unsigned int m_VertexCount	= 0;
		unsigned int m_IndexOffset	= 0;
		unsigned int m_IndexCount	= 0;
		
		Model* m_pModel				= nullptr;
	};
}