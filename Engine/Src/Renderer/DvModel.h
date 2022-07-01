#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class VertexBuffer;
	class DvIndexBuffer;
	class DvMesh;

	class DvModel : public Resource
	{
		RESOURCE_CLASS_TYPE(Model)

	public:

		bool LoadFromFile(const std::string& filepath) override;
		bool SaveToFile(const std::string& filepath) override;

		void Save();


		bool SetMesh(unsigned int index, DvMesh* pMesh);	// lod level�� �ϴ� ����

		unsigned int GetMeshCount() const { return static_cast<unsigned int>(m_Meshes.size()); }
		void SetMeshCount(unsigned int count);

		bool SetIndexBuffers(const std::vector<DvIndexBuffer*>& buffers);

	private:
	private:
		std::vector<VertexBuffer*> m_VertexBuffers;
		std::vector<DvIndexBuffer*> m_IndexBuffers;
		//std::vector<std::vector<DvMesh*>> m_Meshes;	// �ε���, lod level�� ������ �ſ���.
		std::vector<DvMesh*> m_Meshes;
		std::vector<DirectX::XMFLOAT3> m_MeshCenters;
	};
}
