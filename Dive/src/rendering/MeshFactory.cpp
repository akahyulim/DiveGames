#include "stdafx.h"
#include "MeshFactory.h"
#include "Mesh.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Math/Math.h"

using namespace DirectX;

namespace Dive
{
	constexpr float TRIANGLE_SIZE = 1.0f;
	constexpr float CUBE_SIZE = 1.0f;
	constexpr float QUAD_SIZE = 1.0f;
	constexpr float PLANE_SIZE = 10.0f;
	constexpr float SPHERE_RADIUS = 0.5f;
	constexpr float CAPSULE_RADIUS = 0.5f;
	constexpr float CAPSULE_HEIGHT = 1.0f;


	std::shared_ptr<Mesh> MeshFactory::CreateTriangle()
	{
		std::vector<VertexStatic> vertices;
		vertices.resize(3);

		vertices[0] = {
			XMFLOAT3(-TRIANGLE_SIZE / 2.0f, -TRIANGLE_SIZE / 2.0f, 0.0f),
			XMFLOAT2(0.0f, 1.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f) };

		vertices[1] = {
			XMFLOAT3(0.0f, TRIANGLE_SIZE / 2.0f, 0.0f),
			XMFLOAT2(0.5f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f) };
		
		vertices[2] = {
			XMFLOAT3(TRIANGLE_SIZE / 2.0f, -TRIANGLE_SIZE / 2.0f, 0.0f),
			XMFLOAT2(1.0f, 1.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f) };

		std::vector<UINT32> indices;
		indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

		auto mesh = std::make_shared<Mesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->ComputeBouingBox();
		mesh->CreateBuffers();	
		mesh->SetName("TriangleMesh");

		return mesh;
	}

	std::shared_ptr<Mesh> MeshFactory::CreateQuad()
	{
		std::vector<VertexStatic> vertices;
		vertices.resize(4);

		vertices[0] = {
			XMFLOAT3(-QUAD_SIZE / 2.0f, -QUAD_SIZE / 2.0f, -QUAD_SIZE / 2.0f),
			XMFLOAT2(0.0f, 1.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f) };

		vertices[1] = {
			XMFLOAT3(-QUAD_SIZE / 2.0f, QUAD_SIZE / 2.0f, -QUAD_SIZE / 2.0f), 
			XMFLOAT2(0.0f, 0.0f), 
			XMFLOAT3(0.0f, 0.0f, -1.0f), 
			XMFLOAT3(1.0f, 0.0f, 0.0f), 
			XMFLOAT3(0.0f, 1.0f, 0.0f) };

		vertices[2] = {
			XMFLOAT3(QUAD_SIZE / 2.0f, -QUAD_SIZE / 2.0f, -QUAD_SIZE / 2.0f),
			XMFLOAT2(1.0f, 1.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f) };

		vertices[3] = { 
			XMFLOAT3(QUAD_SIZE / 2.0f, QUAD_SIZE / 2.0f, -QUAD_SIZE / 2.0f), 
			XMFLOAT2(1.0f, 0.0f), 
			XMFLOAT3(0.0f, 0.0f, -1.0f), 
			XMFLOAT3(1.0f, 0.0f, 0.0f), 
			XMFLOAT3(0.0f, 1.0f, 0.0f) };
		
		std::vector<UINT32> indices;
		indices.resize(6);
		indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
		indices[3] = 2;	indices[4] = 1;	indices[5] = 3;
		
		auto mesh = std::make_shared<Mesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->ComputeBouingBox();
		mesh->CreateBuffers();
		mesh->SetName("QuadMesh");
		
		return mesh;
	}

	std::shared_ptr<Mesh> MeshFactory::CreatePlane()
	{
		std::vector<VertexStatic> vertices;
		vertices.resize(4);

		vertices[0] = {
			DirectX::XMFLOAT3(-PLANE_SIZE / 2.0f, 0.0f, PLANE_SIZE / 2.0f),
			DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) };

		vertices[1] = {
			DirectX::XMFLOAT3(PLANE_SIZE / 2.0f, 0.0f, PLANE_SIZE / 2.0f),
			DirectX::XMFLOAT2(1.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) };

		vertices[2] = {
			DirectX::XMFLOAT3(-PLANE_SIZE / 2.0f, 0.0f, -PLANE_SIZE / 2.0f),
			DirectX::XMFLOAT2(0.0f, 1.0f),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) };

		vertices[3] = {
			DirectX::XMFLOAT3(PLANE_SIZE / 2.0f, 0.0f, -PLANE_SIZE / 2.0f),
			DirectX::XMFLOAT2(1.0f, 1.0f),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) };

		std::vector<UINT32> indices;
		indices.resize(6);

		indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
		indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

		auto mesh = std::make_shared<Mesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->ComputeBouingBox();
		mesh->CreateBuffers();
		mesh->SetName("PlaneMesh");

		return mesh;
	}

	std::shared_ptr<Mesh> MeshFactory::CreateCube()
	{
		std::vector<VertexStatic> vertices;

		// 전면
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

		// 후면
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));

		// 위
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));

		// 아래
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));

		// 왼쪽
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

		// 오른쪽
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));

		std::vector<UINT32> indices;

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

		auto mesh = std::make_shared<Mesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->ComputeBouingBox();
		mesh->CreateBuffers();
		mesh->SetName("CubeMesh");

		return mesh;
	}

	std::shared_ptr<Mesh> MeshFactory::CreateSphere()
	{
		std::vector<VertexStatic> vertices;
		constexpr int latitudeBands = 30;
		constexpr int longitudeBands = 30;
		vertices.reserve((latitudeBands + 1) * (longitudeBands + 1));
		for (int latNumber = 0; latNumber <= latitudeBands; ++latNumber)
		{
			float theta = latNumber * XM_PI / latitudeBands;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);
			for (int longNumber = 0; longNumber <= longitudeBands; ++longNumber)
			{
				float phi = longNumber * 2.0f * XM_PI / longitudeBands;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);
				XMFLOAT3 position(
					SPHERE_RADIUS * cosPhi * sinTheta,
					SPHERE_RADIUS * cosTheta,
					SPHERE_RADIUS * sinPhi * sinTheta
				);
				XMFLOAT2 texCoords(longNumber / static_cast<float>(longitudeBands), latNumber / static_cast<float>(latitudeBands));
				XMFLOAT3 normal(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
				XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&normal)));
				XMFLOAT3 tangent(-sinPhi, 0.0f, cosPhi);
				vertices.emplace_back(position, texCoords, normal, tangent, XMFLOAT3(0.0f, 1.0f, 0.0f));
			}
		}

		std::vector<UINT32> indices;
		for (int latNumber = 0; latNumber < latitudeBands; ++latNumber)
		{
			for (int longNumber = 0; longNumber < longitudeBands; ++longNumber)
			{
				int first = (latNumber * (longitudeBands + 1)) + longNumber;
				int second = first + longitudeBands + 1;
				indices.emplace_back(first);
				indices.emplace_back(second);
				indices.emplace_back(first + 1);
				indices.emplace_back(second);
				indices.emplace_back(second + 1);
				indices.emplace_back(first + 1);
			}
		}

		auto mesh = std::make_shared<Mesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->ComputeBouingBox();
		mesh->CreateBuffers();
		mesh->SetName("SphereMesh");

		return mesh;
	}

	std::shared_ptr<Mesh> MeshFactory::CreateCapsule()
	{
		std::vector<VertexStatic> vertices;
		constexpr int latitudeBands = 30;
		constexpr int longitudeBands = 30;
		vertices.reserve((latitudeBands + 1) * (longitudeBands + 1) * 2 + (longitudeBands + 1) * 2);

		// 반구 생성
		for (int latNumber = 0; latNumber <= latitudeBands; ++latNumber) 
		{
			float theta = latNumber * XM_PI / latitudeBands;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);

			for (int longNumber = 0; longNumber <= longitudeBands; ++longNumber) 
			{
				float phi = longNumber * 2.0f * XM_PI / longitudeBands;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);

				XMFLOAT3 position(
					CAPSULE_RADIUS * cosPhi * sinTheta,
					CAPSULE_RADIUS * cosTheta + CAPSULE_HEIGHT / 2.0f,
					CAPSULE_RADIUS * sinPhi * sinTheta);

				XMFLOAT2 texCoords(longNumber / static_cast<float>(longitudeBands), latNumber / static_cast<float>(latitudeBands));
				XMFLOAT3 normal(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
				XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&normal)));
				XMFLOAT3 tangent(-sinPhi, 0.0f, cosPhi);

				// 위쪽 반구
				vertices.emplace_back(position, texCoords, normal, tangent, XMFLOAT3(0.0f, 1.0f, 0.0f));

				// 아래쪽 반구
				position.y -= CAPSULE_HEIGHT;
				vertices.emplace_back(position, texCoords, normal, tangent, XMFLOAT3(0.0f, -1.0f, 0.0f));
			}
		}

		// 실린더 부분 추가
		for (int longNumber = 0; longNumber <= longitudeBands; ++longNumber) 
		{
			float phi = longNumber * 2.0f * XM_PI / longitudeBands;
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);

			XMFLOAT3 topPos(CAPSULE_RADIUS * cosPhi, CAPSULE_HEIGHT / 2.0f, CAPSULE_RADIUS * sinPhi);
			XMFLOAT3 bottomPos(CAPSULE_RADIUS * cosPhi, -CAPSULE_HEIGHT / 2.0f, CAPSULE_RADIUS * sinPhi);
			XMFLOAT2 texCoord(longNumber / static_cast<float>(longitudeBands), 0.5f);

			XMFLOAT3 normal(cosPhi, 0.0f, sinPhi);
			XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&normal)));
			XMFLOAT3 tangent(-sinPhi, 0.0f, cosPhi);

			vertices.emplace_back(topPos, texCoord, normal, tangent, XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(bottomPos, texCoord, normal, tangent, XMFLOAT3(0.0f, -1.0f, 0.0f));
		}

		std::vector<UINT32> indices;
		for (int latNumber = 0; latNumber < latitudeBands; ++latNumber) 
		{
			for (int longNumber = 0; longNumber < longitudeBands; ++longNumber) {
				int first = (latNumber * (longitudeBands + 1) * 2) + longNumber * 2;
				int second = first + longitudeBands * 2 + 2;

				// 위쪽 반구
				indices.emplace_back(first);
				indices.emplace_back(second);
				indices.emplace_back(first + 1);
				indices.emplace_back(second);
				indices.emplace_back(second + 1);
				indices.emplace_back(first + 1);

				// 아래쪽 반구
				first += longitudeBands * 2 + 2;
				second += longitudeBands * 2 + 2;

				indices.emplace_back(first);
				indices.emplace_back(second);
				indices.emplace_back(first + 1);
				indices.emplace_back(second);
				indices.emplace_back(second + 1);
				indices.emplace_back(first + 1);
			}
		}

		// 실린더 인덱스 추가
		for (int longNumber = 0; longNumber < longitudeBands; ++longNumber) 
		{
			int first = (latitudeBands + 1) * (longitudeBands + 1) * 2 + longNumber * 2;
			int second = first + 2;

			indices.emplace_back(first);
			indices.emplace_back(second);
			indices.emplace_back(first + 1);
			indices.emplace_back(second);
			indices.emplace_back(second + 1);
			indices.emplace_back(first + 1);
		}

		auto mesh = std::make_shared<Mesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->ComputeBouingBox();
		mesh->CreateBuffers();
		mesh->SetName("CapsuleMesh");

		return mesh;
	}
}
