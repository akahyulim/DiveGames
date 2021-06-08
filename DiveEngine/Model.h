#pragma once
#include "Resource.h"
#include "GraphicsInclude.h"
#include <string>
#include <vector>

namespace Dive
{
	struct dvMesh;

	// 모델은 오브젝트의 부위 메시가 아니다.
	// 하나의 전체 오브젝트(혹은 rigid, skinned로 구분되는 것)임을 염두해야 한다. 
	class Model : public Resource
	{
	public:
		Model();
		~Model();

		void Clear();

		// 이 함수들은 좀 더 생각해보자.
		void LoadFromFile(const std::string& filepath);
		void SaveToFile(const std::string& filepath);

		// 정점 버퍼는 각 요소별로 나뉜다.
		// 따라서 배열로 묶어서 전달해야 한다.
		// Stride, offset도 마찬가지다.
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }
		unsigned int GetBufferCount() { static_cast<unsigned int>(m_vertexBuffers.size()); }

		dvMesh* GetMesh() { return m_pMesh; }
		void SetMesh(dvMesh* pMesh);

	private:
		bool createBuffers();

	private:
		dvMesh* m_pMesh;
		std::vector<ID3D11Buffer*> m_vertexBuffers;
		ID3D11Buffer* m_pIndexBuffer;
		std::vector<unsigned int> m_strides;
		std::vector<unsigned int> m_offsets;

		// bounding box
	};
}