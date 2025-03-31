#include "stdafx.h"
#include "ModelFactory.h"
#include "Mesh.h"
#include "Model.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Math/Math.h"

using namespace DirectX;

namespace Dive
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

		std::vector<UINT32> indices;
		indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

		// vb, ib�� �̷������� ������ �� mesh�� �߰��� �� �ִ�.
		VertexBuffer vb;
		vb.Create(vertices.data(), sizeof(VertexStatic), static_cast<UINT32>(vertices.size()));

		Mesh* mesh = new Mesh;
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->ComputeBouingBox();
		mesh->CreateBuffers();
		mesh->SetName("Triangle");

		Model* model = new Model;
		model->AddMesh(mesh);
		model->SetName("Triangle");
			
		return model;
	}

	Model* CreateCube(float size)
	{
		// vertex buffer
		std::vector<VertexStatic> vertices;

		// ����
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

		// �ĸ�
		vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));

		// ��
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));

		// �Ʒ�
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));

		// ����
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

		// ������
		vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));

		// index buffer
		std::vector<UINT32> indices;

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

		Mesh* mesh = new Mesh;
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->ComputeBouingBox();
		mesh->CreateBuffers();
		mesh->SetName("Cube");
			
		Model* model = new Model;
		model->AddMesh(mesh);
		model->SetName("Cube");

		return model;
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

		std::vector<UINT32> indices;
		indices.resize(6);

		indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
		indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

		Mesh* mesh = new Mesh;
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->ComputeBouingBox();
		mesh->CreateBuffers();
		mesh->SetName("Plane");
			
		Model* model = new Model;
		model->AddMesh(mesh);
		model->SetName("Plane");

		return model;
	}
}