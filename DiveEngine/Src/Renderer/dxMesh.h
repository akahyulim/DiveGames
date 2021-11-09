#pragma once
#include "../Resource/Resource.h"
#include "Graphics/GraphicsInclude.h"
#include "VertexTypes.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace DiveEngine
{
	class GameObject;
	class Transform;

	struct MeshData
	{
		unsigned int offset;
		unsigned int count;
	};

	class dvMesh : public Resource
	{
	public:
	private:
	private:
		std::vector<VertexType_PosTexNorTan> m_Vertices;
		std::vector<unsigned int> m_Indices;

		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;

		std::unordered_map<std::string, MeshData> m_MeshData;
	}
}