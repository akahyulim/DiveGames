#pragma once
#include "Core/Object.h"
#include "Resource/Resource.h"
#include "Graphics/GraphicsDefs.h"

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

		// override
		bool Load(FileStream* pDeserializer) override;
		bool Save(FileStream* pSerializer) override;

		// static
		static void RegisterObject(Context* pContext);

		const std::vector<VertexBuffer*>& GetVertexBuffers() const { return m_VertexBuffers; }
		bool SetVertexBuffers(const std::vector<VertexBuffer*>& pBuffers);

		const std::vector<IndexBuffer*>& GetIndexBuffers() const { return m_IndexBuffers; }
		bool SetIndexBuffers(const std::vector<IndexBuffer*>& pBuffers);

		unsigned int GetMeshCount() const { return static_cast<unsigned int>(m_Meshes.size()); }
		void SetNumMeshes(size_t num);
		
		const std::vector<Mesh*>& GetMeshes() const { return m_Meshes; }
		Mesh* GetMesh(size_t index);
		bool SetMesh(size_t index, Mesh* pMesh);

	private:
	private:
		std::vector<VertexBuffer*> m_VertexBuffers;
		std::vector<IndexBuffer*> m_IndexBuffers;
		std::vector<Mesh*> m_Meshes;

		// root game object
		// - transform과 drawable로 구성된다.
		// - drawable은 mesh에서 자신이 그릴 부분의 정보(name, count, offset 등) + model만 관리한다.
		// - material 역시 drawable이 관리하는 듯 하다.
		// - serialization은 model과 구분된다. scene의 다른 gameobjects들과 함께 파일화했던 것 같다.
		
		// buffers		
		// - mesh를 이용해 이 곳에서 직접 생성한다.
		
		// mesh
		// - 왜인지 모르겠지만 메시가 하나다. 일반적인 형태이긴 하다.
		// - vertices, indices만 관리한다.
	};
}
