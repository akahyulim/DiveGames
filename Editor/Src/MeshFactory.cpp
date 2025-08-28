#include "MeshFactory.h"

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

	std::shared_ptr<StaticMesh> MeshFactory::CreateTriangle()
	{
		std::vector<LitVertex> vertices;
		vertices.resize(3);

		// 좌하단
		vertices[0] = {
			XMFLOAT3(-TRIANGLE_SIZE / 2.0f, -TRIANGLE_SIZE / 2.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 1.0f) };

		// 상단
		vertices[1] = {
			XMFLOAT3(0.0f, TRIANGLE_SIZE / 2.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.5f, 0.0f) };

		// 우하단
		vertices[2] = {
			XMFLOAT3(TRIANGLE_SIZE / 2.0f, -TRIANGLE_SIZE / 2.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 1.0f) };

		std::vector<uint32_t> indices;
		indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

		auto mesh = std::make_shared<StaticMesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->CreateBuffers();
		mesh->ComputeBouingBox();
		mesh->SetName("TriangleMesh");

		return mesh;
	}

	std::shared_ptr<StaticMesh> MeshFactory::CreateQuad()
	{
		std::vector<LitVertex> vertices;
		vertices.resize(4);

		// 좌하단
		vertices[0] = {
			XMFLOAT3(-QUAD_SIZE / 2.0f, -QUAD_SIZE / 2.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 1.0f) };

		// 좌상단
		vertices[1] = {
			XMFLOAT3(-QUAD_SIZE / 2.0f, QUAD_SIZE / 2.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 0.0f) };

		// 우하단
		vertices[2] = {
			XMFLOAT3(QUAD_SIZE / 2.0f, -QUAD_SIZE / 2.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 1.0f) };

		// 우상단
		vertices[3] = {
			XMFLOAT3(QUAD_SIZE / 2.0f, QUAD_SIZE / 2.0f, 0.0f),
			XMFLOAT3(0.0f, 0.0f, -1.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 0.0f) };

		std::vector<uint32_t> indices;
		indices.resize(6);
		indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
		indices[3] = 2;	indices[4] = 1;	indices[5] = 3;

		auto mesh = std::make_shared<StaticMesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->CreateBuffers();
		mesh->ComputeBouingBox();
		mesh->SetName("QuadMesh");

		return mesh;
	}

	std::shared_ptr<StaticMesh> MeshFactory::CreatePlane()
	{
		std::vector<LitVertex> vertices;
		vertices.resize(4);

		// 좌후면
		vertices[0] = {
			XMFLOAT3(-PLANE_SIZE / 2.0f, 0.0f, PLANE_SIZE / 2.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 0.0f) };

		// 우후면
		vertices[1] = {
			XMFLOAT3(PLANE_SIZE / 2.0f, 0.0f, PLANE_SIZE / 2.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 0.0f) };

		// 좌전면
		vertices[2] = {
			XMFLOAT3(-PLANE_SIZE / 2.0f, 0.0f, -PLANE_SIZE / 2.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(0.0f, 1.0f) };

		// 우전면
		vertices[3] = {
			XMFLOAT3(PLANE_SIZE / 2.0f, 0.0f, -PLANE_SIZE / 2.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(1.0f, 0.0f, 0.0f),
			XMFLOAT2(1.0f, 1.0f) };

		std::vector<uint32_t> indices;
		indices.resize(6);

		indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
		indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

		auto mesh = std::make_shared<StaticMesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->CreateBuffers();
		mesh->ComputeBouingBox();
		mesh->SetName("PlaneMesh");

		return mesh;
	}

	std::shared_ptr<StaticMesh> MeshFactory::CreateCube()
	{
		std::vector<LitVertex> vertices;

		// 전면
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));

		// 후면
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)
			, XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));

		// 위
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));

		// 아래
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(0.0f, -1.0f, 0.0f)
			, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f));

		// 왼쪽
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(-CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f));

		// 오른쪽
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, -CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f));
		vertices.emplace_back(XMFLOAT3(CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f, CUBE_SIZE / 2.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
			, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f));

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

		auto mesh = std::make_shared<StaticMesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->CreateBuffers();
		mesh->ComputeBouingBox();
		mesh->SetName("CubeMesh");

		return mesh;
	}

	std::shared_ptr<StaticMesh> MeshFactory::CreateSphere()
	{
		constexpr int latitudeBands = 30;
		constexpr int longitudeBands = 30;
		constexpr float radius = SPHERE_RADIUS;

		std::vector<LitVertex> vertices;
		std::vector<uint32_t> indices;

		XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);
		XMFLOAT3 center(0.0f, 0.0f, 0.0f);

		// 정점 생성
		for (int lat = 0; lat <= latitudeBands; ++lat)
		{
			float theta = XM_PI * (lat / static_cast<float>(latitudeBands)); // 0 ~ π
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);

			for (int lon = 0; lon <= longitudeBands; ++lon)
			{
				float phi = 2.0f * XM_PI * (lon / static_cast<float>(longitudeBands)); // 0 ~ 2π
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);

				XMFLOAT3 offset(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
				XMFLOAT3 position(
					radius * offset.x,
					radius * offset.y,
					radius * offset.z);

				XMVECTOR posVec = XMLoadFloat3(&position);
				XMVECTOR centerVec = XMLoadFloat3(&center);
				XMVECTOR normalVec = XMVector3Normalize(posVec - centerVec);
				XMFLOAT3 normal;
				XMStoreFloat3(&normal, normalVec);

				XMVECTOR tangentVec = XMVector3Normalize(XMVector3Cross(normalVec, upVec));
				XMFLOAT3 tangent;
				XMStoreFloat3(&tangent, tangentVec);

				float u = lon / static_cast<float>(longitudeBands);
				float v = lat / static_cast<float>(latitudeBands);

				vertices.emplace_back(position, normal, tangent, XMFLOAT2(u, v));
			}
		}

		// 인덱스 연결
		auto connectBands = [&](int start, int bands) {
			for (int lat = 0; lat < bands; ++lat)
			{
				for (int lon = 0; lon < longitudeBands; ++lon)
				{
					int i0 = start + lat * (longitudeBands + 1) + lon;
					int i1 = i0 + 1;
					int i2 = i0 + (longitudeBands + 1);
					int i3 = i2 + 1;

					indices.push_back(i0);
					indices.push_back(i1);
					indices.push_back(i2);
					indices.push_back(i1);
					indices.push_back(i3);
					indices.push_back(i2);
				}
			}
			};

		int startIndex = 0;
		connectBands(startIndex, latitudeBands);

		// 메시 생성
		auto mesh = std::make_shared<StaticMesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->CreateBuffers();
		mesh->ComputeBouingBox();
		mesh->SetName("SphereMesh");

		return mesh;
	}


	std::shared_ptr<StaticMesh> MeshFactory::CreateCapsule()
	{
		constexpr int latitudeBands = 30;
		constexpr int longitudeBands = 30;
		constexpr float radius = CAPSULE_RADIUS;
		constexpr float height = CAPSULE_HEIGHT;

		std::vector<LitVertex> vertices;
		std::vector<uint32_t> indices;

		XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);
		XMFLOAT3 centerTop(0.0f, height / 2.0f, 0.0f);
		XMFLOAT3 centerBottom(0.0f, -height / 2.0f, 0.0f);

		// 위쪽 반구
		for (int lat = 0; lat <= latitudeBands; ++lat)
		{
			float theta = XM_PI / 2.0f * (lat / static_cast<float>(latitudeBands));
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);

			for (int lon = 0; lon <= longitudeBands; ++lon)
			{
				float phi = 2.0f * XM_PI * (lon / static_cast<float>(longitudeBands));
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);

				XMFLOAT3 offset(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
				XMFLOAT3 position(
					radius * offset.x,
					centerTop.y + radius * offset.y,
					radius * offset.z);

				XMVECTOR posVec = XMLoadFloat3(&position);
				XMVECTOR centerVec = XMLoadFloat3(&centerTop);
				XMVECTOR normalVec = XMVector3Normalize(posVec - centerVec);
				XMFLOAT3 normal;
				XMStoreFloat3(&normal, normalVec);

				XMVECTOR tangentVec = XMVector3Normalize(XMVector3Cross(normalVec, upVec));
				XMFLOAT3 tangent;
				XMStoreFloat3(&tangent, tangentVec);

				float u = lon / static_cast<float>(longitudeBands);
				float v = lat / static_cast<float>(latitudeBands) * 0.25f;

				vertices.emplace_back(position, normal, tangent, XMFLOAT2(u, v));
			}
		}

		// 실린더
		for (int yStep = 0; yStep <= 1; ++yStep)
		{
			float y = centerTop.y - yStep * height;

			for (int lon = 0; lon <= longitudeBands; ++lon)
			{
				float phi = 2.0f * XM_PI * (lon / static_cast<float>(longitudeBands));
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);

				XMFLOAT3 position(radius * cosPhi, y, radius * sinPhi);
				XMFLOAT3 normal(cosPhi, 0.0f, sinPhi);

				XMVECTOR normalVec = XMVector3Normalize(XMLoadFloat3(&normal));
				XMVECTOR tangentVec = XMVector3Normalize(XMVector3Cross(normalVec, upVec));
				XMFLOAT3 tangent;
				XMStoreFloat3(&tangent, tangentVec);

				float u = lon / static_cast<float>(longitudeBands);
				float v = 0.25f + yStep * 0.5f;

				vertices.emplace_back(position, normal, tangent, XMFLOAT2(u, v));
			}
		}

		// 아래쪽 반구
		for (int lat = 0; lat <= latitudeBands; ++lat)
		{
			float theta = XM_PI / 2.0f * (lat / static_cast<float>(latitudeBands));
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);

			for (int lon = 0; lon <= longitudeBands; ++lon)
			{
				float phi = 2.0f * XM_PI * (lon / static_cast<float>(longitudeBands));
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);

				XMFLOAT3 offset(cosPhi * sinTheta, -cosTheta, sinPhi * sinTheta);
				XMFLOAT3 position(
					radius * offset.x,
					centerBottom.y + radius * offset.y,
					radius * offset.z);

				XMVECTOR posVec = XMLoadFloat3(&position);
				XMVECTOR centerVec = XMLoadFloat3(&centerBottom);
				XMVECTOR normalVec = XMVector3Normalize(posVec - centerVec);
				XMFLOAT3 normal;
				XMStoreFloat3(&normal, normalVec);

				XMVECTOR tangentVec = XMVector3Normalize(XMVector3Cross(normalVec, upVec));
				XMFLOAT3 tangent;
				XMStoreFloat3(&tangent, tangentVec);

				float u = lon / static_cast<float>(longitudeBands);
				float v = 0.75f + lat / static_cast<float>(latitudeBands) * 0.25f;

				vertices.emplace_back(position, normal, tangent, XMFLOAT2(u, v));
			}
		}

		// 인덱스 연결 함수들
		auto connectBands = [&](int start, int bands) {
			for (int lat = 0; lat < bands; ++lat)
			{
				for (int lon = 0; lon < longitudeBands; ++lon)
				{
					int i0 = start + lat * (longitudeBands + 1) + lon;
					int i1 = i0 + 1;
					int i2 = i0 + (longitudeBands + 1);
					int i3 = i2 + 1;

					indices.push_back(i0);
					indices.push_back(i1);
					indices.push_back(i2);
					indices.push_back(i1);
					indices.push_back(i3);
					indices.push_back(i2);
				}
			}
			};

		auto connectBandsReversed = [&](int start, int bands) {
			for (int lat = 0; lat < bands; ++lat)
			{
				for (int lon = 0; lon < longitudeBands; ++lon)
				{
					int i0 = start + lat * (longitudeBands + 1) + lon;
					int i1 = i0 + 1;
					int i2 = i0 + (longitudeBands + 1);
					int i3 = i2 + 1;

					indices.push_back(i0);
					indices.push_back(i2);
					indices.push_back(i1);
					indices.push_back(i1);
					indices.push_back(i2);
					indices.push_back(i3);
				}
			}
			};

		// 인덱스 연결
		int topStart = 0;
		int cylinderStart = topStart + (latitudeBands + 1) * (longitudeBands + 1);
		int bottomStart = cylinderStart + 2 * (longitudeBands + 1);

		connectBands(topStart, latitudeBands);               // 위쪽 반구
		connectBands(cylinderStart, 1);                      // 실린더
		connectBandsReversed(bottomStart, latitudeBands);    // 아래쪽 반구 (반전)

		// 메시 생성
		auto mesh = std::make_shared<StaticMesh>();
		mesh->AddVertices(vertices);
		mesh->AddIndices(indices);
		mesh->CreateBuffers();
		mesh->ComputeBouingBox();
		mesh->SetName("CapsuleMesh");

		return mesh;
	}

	std::shared_ptr<StaticMesh> MeshFactory::Create(const std::string& meshName)
	{
		if (meshName == "TriangleMesh")	return CreateTriangle();
		if (meshName == "QuadMesh")		return CreateQuad();
		if (meshName == "PlaneMesh")	return CreatePlane();
		if (meshName == "CubeMesh")		return CreateCube();
		if (meshName == "SphereMesh")	return CreateSphere();
		if (meshName == "CapsuleMesh")	return CreateCapsule();

		return nullptr;
	}
}
