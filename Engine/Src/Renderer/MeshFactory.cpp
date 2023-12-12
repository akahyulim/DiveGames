#include "divepch.h"
#include "MeshFactory.h"

namespace Dive
{
	StaticMesh* CreateQuad(float width, float height)
	{
		return nullptr;
	}

	StaticMesh* CreateCube(float size)
	{
		std::vector<VertexStatic> vertices;
		vertices.resize(8);

		vertices[0] = {
			DirectX::XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f),
			DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) };



		return nullptr;
	}
	
	StaticMesh* CreatePlane(float width, float height)
	{
		std::vector<VertexStatic> vertices;
		vertices.resize(4);

		vertices[0] = {
			DirectX::XMFLOAT3(-width / 2.0f, 0.0f, height / 2.0f),
			DirectX::XMFLOAT2(0.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) };

		vertices[1] = {
			DirectX::XMFLOAT3(width / 2.0f, 0.0f, height / 2.0f),
			DirectX::XMFLOAT2(1.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) };

		vertices[2] = {
			DirectX::XMFLOAT3(-width / 2.0f, 0.0f, -height / 2.0f),
			DirectX::XMFLOAT2(0.0f, 1.0f),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) };

		vertices[3] = {
			DirectX::XMFLOAT3(width / 2.0f, 0.0f, -height / 2.0f),
			DirectX::XMFLOAT2(1.0f, 1.0f),
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) };

		std::vector<uint32_t> indices;
		indices.resize(6);

		indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
		indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

		auto pPlane =  new StaticMesh("Plane", vertices, indices);
		pPlane->CreateVertexBuffer();
		pPlane->CreateIndexBuffer();

		return pPlane;
	}

	StaticMesh* CreateSphere(float radius, int slices, int stacks)
	{
		std::vector<VertexStatic> vertices;

		// 위쪽 꼭지점
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
				//nor;
				//tan; = { -radius * sinf(phi) * sinf(theta), 0.0f, sinf(phi) * cosf(theta) };

				vertices.emplace_back(pos, uv, nor, tan);
			}
		}

		// 아래쪽 꼭지점
		pos = { 0.0f, -radius, 0.0f };
		uv = {0.0f, 1.0f};
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

		auto pSphere = new StaticMesh("Sphere", vertices, indices);
		pSphere->CreateVertexBuffer();
		pSphere->CreateIndexBuffer();

		return pSphere;
	}
}