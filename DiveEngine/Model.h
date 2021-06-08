#pragma once
#include "Resource.h"
#include "GraphicsInclude.h"
#include <string>
#include <vector>

namespace Dive
{
	struct dvMesh;

	// ���� ������Ʈ�� ���� �޽ð� �ƴϴ�.
	// �ϳ��� ��ü ������Ʈ(Ȥ�� rigid, skinned�� ���еǴ� ��)���� �����ؾ� �Ѵ�. 
	class Model : public Resource
	{
	public:
		Model();
		~Model();

		void Clear();

		// �� �Լ����� �� �� �����غ���.
		void LoadFromFile(const std::string& filepath);
		void SaveToFile(const std::string& filepath);

		// ���� ���۴� �� ��Һ��� ������.
		// ���� �迭�� ��� �����ؾ� �Ѵ�.
		// Stride, offset�� ����������.
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