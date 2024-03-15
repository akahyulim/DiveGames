#include "divepch.h"
#include "ModelFactory.h"
#include "Mesh.h"
#include "Model.h"
#include "Graphics/GraphicsDefs.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Core/CoreDefs.h"
#include "Scene/SceneManager.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Renderable.h"

using namespace DirectX;

namespace Dive
{
	namespace ModelFactory
	{
		Model* CreateTriangle(float size)
		{
			std::vector<VertexStatic> vertices;
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(0.0f, size / 2.0f, 0.0f), XMFLOAT2(0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));

			std::vector<uint32_t> indices;
			indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

			Mesh* pMesh = new Mesh;
			uint32_t vertexOffset = 0;
			pMesh->AddVertices(vertices);
			pMesh->AddIndices(indices);
			pMesh->SetName("Triangle");

			Model* pModel = new Model;
			pModel->AddMesh(pMesh);
			pModel->SetName("Triangle");
			pModel->GenerateBuffers();
			
			return pModel;
		}

		Model* CreateCube(float size)
		{
			// vertex buffer
			std::vector<VertexStatic> vertices;

			// ����
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));

			// �ĸ�
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));

			// ��
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

			// �Ʒ�
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));

			// ����
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));

			// ������
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));

			// index buffer
			std::vector<uint32_t> indices;

			// ����
			indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);
			indices.emplace_back(2);	indices.emplace_back(1);	indices.emplace_back(3);

			// �ĸ�
			indices.emplace_back(4);	indices.emplace_back(5);	indices.emplace_back(6);
			indices.emplace_back(6);	indices.emplace_back(5);	indices.emplace_back(7);

			// ��
			indices.emplace_back(8);	indices.emplace_back(9);	indices.emplace_back(10);
			indices.emplace_back(10);	indices.emplace_back(9);	indices.emplace_back(11);

			// �Ʒ�
			indices.emplace_back(12);	indices.emplace_back(13);	indices.emplace_back(14);
			indices.emplace_back(14);	indices.emplace_back(13);	indices.emplace_back(15);

			// ����
			indices.emplace_back(16);	indices.emplace_back(17);	indices.emplace_back(18);
			indices.emplace_back(18);	indices.emplace_back(17);	indices.emplace_back(19);

			// ������
			indices.emplace_back(20);	indices.emplace_back(21);	indices.emplace_back(22);
			indices.emplace_back(22);	indices.emplace_back(21);	indices.emplace_back(23);

			Mesh* pMesh = new Mesh;
			pMesh->AddVertices(vertices);
			pMesh->AddIndices(indices);
			pMesh->SetName("Cube");
			
			Model* pModel = new Model;
			pModel->AddMesh(pMesh);
			pModel->SetName("Cube");
			pModel->GenerateBuffers();

			return pModel;
		}

		Model* CreatePlane(float length, float depth)
		{
			std::vector<VertexStatic> vertices;
			vertices.resize(4);

			vertices[0] = {
				DirectX::XMFLOAT3(-length / 2.0f, 0.0f, depth / 2.0f),
				DirectX::XMFLOAT2(0.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) };

			vertices[1] = {
				DirectX::XMFLOAT3(length / 2.0f, 0.0f, depth / 2.0f),
				DirectX::XMFLOAT2(1.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) };

			vertices[2] = {
				DirectX::XMFLOAT3(-length / 2.0f, 0.0f, -depth / 2.0f),
				DirectX::XMFLOAT2(0.0f, 1.0f),
				DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) };

			vertices[3] = {
				DirectX::XMFLOAT3(length / 2.0f, 0.0f, -depth / 2.0f),
				DirectX::XMFLOAT2(1.0f, 1.0f),
				DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) };

			std::vector<uint32_t> indices;
			indices.resize(6);

			indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
			indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

			Mesh* pMesh = new Mesh;
			pMesh->AddVertices(vertices);
			pMesh->AddIndices(indices);
			pMesh->SetName("Plane");
			
			Model* pModel = new Model;
			pModel->AddMesh(pMesh);
			pModel->SetName("Plane");
			pModel->GenerateBuffers();

			return pModel;
		}

		Model* CreateSphere(float radius, int slices, int stacks)
		{
			std::vector<VertexStatic> vertices;

			// ���� ������
			DirectX::XMFLOAT3 pos = { 0.0f, radius, 0.0f };
			DirectX::XMFLOAT2 uv = { 0.0f, 0.0f };
			DirectX::XMFLOAT3 nor = { 0.0f, 1.0f, 0.0f };
			DirectX::XMFLOAT3 tan = { 1.0f, 0.0f, 0.0f };
			vertices.emplace_back(pos, uv, nor, tan);

			const float phiStep = DirectX::XM_PI / stacks;
			const float thetaStep = 2.0f * DirectX::XM_PI / slices;

			for (int i = 1; i <= stacks - 1; ++i)
			{
				const float phi = i * phiStep;
				for (int j = 0; j <= slices; ++j)
				{
					const float theta = j * thetaStep;

					pos = { radius * sinf(phi) * cosf(theta), radius * cosf(phi), radius * sinf(phi) * sinf(theta) };
					uv = { theta / (DirectX::XM_PI * 2), phi / DirectX::XM_PI };
					auto n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&pos));
					DirectX::XMStoreFloat3(&nor, n);
					tan = { -radius * sinf(phi) * sinf(theta), 0.0f, sinf(phi) * cosf(theta) };
					auto t = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&tan));
					DirectX::XMStoreFloat3(&tan, t);

					vertices.emplace_back(pos, uv, nor, tan);
				}
			}

			// �Ʒ��� ������
			pos = { 0.0f, -radius, 0.0f };
			uv = { 0.0f, 1.0f };
			nor = { 0.0f, -1.0f, 0.0f };
			tan = { 1.0f, 0.0f, 0.0f };
			vertices.emplace_back(pos, uv, nor, tan);

			std::vector<uint32_t> indices;

			for (int i = 1; i <= slices; i++)
			{
				indices.emplace_back(0);
				indices.emplace_back(i + 1);
				indices.emplace_back(i);
			}
			int baseIndex = 1;
			const int ringVertexCount = slices + 1;
			for (int i = 0; i < stacks - 2; i++)
			{
				for (int j = 0; j < slices; j++)
				{
					indices.emplace_back(baseIndex + i * ringVertexCount + j);
					indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
					indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);

					indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);
					indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
					indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
				}
			}
			int southPoleIndex = static_cast<int>(vertices.size()) - 1;
			baseIndex = southPoleIndex - ringVertexCount;
			for (int i = 0; i < slices; i++)
			{
				indices.emplace_back(southPoleIndex);
				indices.emplace_back(baseIndex + i);
				indices.emplace_back(baseIndex + i + 1);
			}

			Mesh* pMesh = new Mesh;
			pMesh->AddVertices(vertices);
			pMesh->AddIndices(indices);
			pMesh->SetName("Sphere");
			
			Model* pModel = new Model;
			pModel->AddMesh(pMesh);
			pModel->SetName("Sphere");
			pModel->GenerateBuffers();

			return pModel;
		}
	}
}