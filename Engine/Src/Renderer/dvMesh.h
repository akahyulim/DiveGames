#pragma once
#include "../Resource/Resource.h"
#include "Graphics/GraphicsInclude.h"
#include "VertexTypes.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace Dive
{
	class GameObject;
	class Transform;

	// Interface라기보단 base class일 수 밖에 없다.
	class dvMesh : public Resource
	{
	public:
		dvMesh();
		virtual ~dvMesh();

		virtual bool CreateBuffers(ID3D11Device* pDevice) = 0;
		virtual void Clear() = 0;

		// GetAttribute
		virtual uint32_t GetVertexCount() = 0;
		virtual size_t GetVertexStride() = 0;
		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }

		virtual uint32_t GetIndexCount() { return m_indices.size(); }
		virtual DXGI_FORMAT GetIndexFormat() = 0; 
		virtual size_t GetIndexStride() = 0;
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }

		void SetIndices(const std::vector<uint32_t>& indices);

	private:

	protected:
		std::vector<uint32_t> m_indices;
		
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;

		// subMesh
	};
}