#pragma once
#include "Core/Object.h"
#include "Resource/Resource.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Context;
	class VertexBuffer;
	class IndexBuffer;
	class Mesh;
	class FileStream;

	class Model : public Resource
	{
		DIVE_OBJECT(Model, Resource)

	public:
		explicit Model(Context* pContext);
		~Model() override;

		static void RegisterObject(Context* pContext);

		bool Load(FileStream* pDeserializer) override;
		bool Save(FileStream* pSerializer) override;

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
