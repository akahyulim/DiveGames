#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class VertexBuffer;
	class IndexBuffer;
	class Mesh;

	// 추후 Resource로 만들어야 한다.
	class Model : public Object
	{
		DIVE_OBJECT(Model, Object)

	public:
		explicit Model(Context* pContext);
		~Model() override;

		// 아무래도 Reosurce에서 override되어야 할 듯...
		bool Load();
		bool Save();

		const std::vector<VertexBuffer*>& GetVertexBuffers() const { return m_VertexBuffers; }
		bool SetVertexBuffers(const std::vector<VertexBuffer*>& pBuffers);

		const std::vector<IndexBuffer*>& GetIndexBuffers() const { return m_IndexBuffers; }
		bool SetIndexBuffers(const std::vector<IndexBuffer*>& pBuffers);

		unsigned int GetNumMeshes() const { return (unsigned int)m_Meshes.size(); }
		void SetNumMeshes(unsigned int num);
		
		const std::vector<Mesh*>& GetMeshes() const { return m_Meshes; }
		Mesh* GetMesh(unsigned int index);
		bool SetMesh(unsigned int index, Mesh* pMesh);

	private:
	private:
		std::vector<VertexBuffer*> m_VertexBuffers;
		std::vector<IndexBuffer*> m_IndexBuffers;
		std::vector<Mesh*> m_Meshes;
	};
}
