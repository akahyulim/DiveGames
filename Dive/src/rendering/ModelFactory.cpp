#include "divepch.h"
#include "ModelFactory.h"
#include "Mesh.h"
#include "Model.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Math/Math.h"
#include "World/SceneManager.h"
#include "World/GameObject.h"
#include "World/Components/Renderable.h"

using namespace DirectX;

namespace Dive
{
	namespace ModelFactory
	{
		Model* CreateTriangle(float size)
		{
			std::vector<VertexStatic> vertices;
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(0.0f, size / 2.0f, 0.0f), XMFLOAT2(0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

			std::vector<uint32_t> indices;
			indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

			// vb, ib는 이런식으로 생성한 후 mesh에 추가할 수 있다.
			VertexBuffer vb;
			vb.CreateBuffer(vertices.data(), sizeof(VertexStatic), static_cast<uint32_t>(vertices.size()));

			Mesh* pMesh = new Mesh;
			pMesh->AddVertices(vertices);
			pMesh->AddIndices(indices);
			pMesh->ComputeBouingBox();
			pMesh->CreateBuffers();
			pMesh->SetName("Triangle");

			Model* pModel = new Model;
			pModel->AddMesh(pMesh);
			pModel->SetName("Triangle");
			
			return pModel;
		}

		Model* CreateCube(float size)
		{
			// vertex buffer
			std::vector<VertexStatic> vertices;

			// 전면
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

			// 후면
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
				, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
				, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
				, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
				, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));

			// 위
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));

			// 아래
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
				, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));

			// 왼쪽
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
				, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
				, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
				, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
				, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

			// 오른쪽
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
				, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
				, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
				, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
				, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));

			// index buffer
			std::vector<uint32_t> indices;

			// 전면
			indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);
			indices.emplace_back(2);	indices.emplace_back(1);	indices.emplace_back(3);

			// 후면
			indices.emplace_back(4);	indices.emplace_back(5);	indices.emplace_back(6);
			indices.emplace_back(6);	indices.emplace_back(5);	indices.emplace_back(7);

			// 위
			indices.emplace_back(8);	indices.emplace_back(9);	indices.emplace_back(10);
			indices.emplace_back(10);	indices.emplace_back(9);	indices.emplace_back(11);

			// 아래
			indices.emplace_back(12);	indices.emplace_back(13);	indices.emplace_back(14);
			indices.emplace_back(14);	indices.emplace_back(13);	indices.emplace_back(15);

			// 왼쪽
			indices.emplace_back(16);	indices.emplace_back(17);	indices.emplace_back(18);
			indices.emplace_back(18);	indices.emplace_back(17);	indices.emplace_back(19);

			// 오른쪽
			indices.emplace_back(20);	indices.emplace_back(21);	indices.emplace_back(22);
			indices.emplace_back(22);	indices.emplace_back(21);	indices.emplace_back(23);

			Mesh* pMesh = new Mesh;
			pMesh->AddVertices(vertices);
			pMesh->AddIndices(indices);
			pMesh->ComputeBouingBox();
			pMesh->CreateBuffers();
			pMesh->SetName("Cube");
			
			Model* pModel = new Model;
			pModel->AddMesh(pMesh);
			pModel->SetName("Cube");

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
				DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) };

			vertices[1] = {
				DirectX::XMFLOAT3(length / 2.0f, 0.0f, depth / 2.0f),
				DirectX::XMFLOAT2(1.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
				DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) };

			vertices[2] = {
				DirectX::XMFLOAT3(-length / 2.0f, 0.0f, -depth / 2.0f),
				DirectX::XMFLOAT2(0.0f, 1.0f),
				DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
				DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) };

			vertices[3] = {
				DirectX::XMFLOAT3(length / 2.0f, 0.0f, -depth / 2.0f),
				DirectX::XMFLOAT2(1.0f, 1.0f),
				DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
				DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
				DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) };

			std::vector<uint32_t> indices;
			indices.resize(6);

			indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
			indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

			Mesh* pMesh = new Mesh;
			pMesh->AddVertices(vertices);
			pMesh->AddIndices(indices);
			pMesh->ComputeBouingBox();
			pMesh->CreateBuffers();
			pMesh->SetName("Plane");
			
			Model* pModel = new Model;
			pModel->AddMesh(pMesh);
			pModel->SetName("Plane");

			return pModel;
		}
	}
}