#include "divepch.h"
#include "DvMeshFactory.h"
#include "DvMesh.h"
#include "Graphics/GraphicsDefs.h"
#include "Graphics/DvVertexBuffer.h"
#include "Graphics/DvIndexBuffer.h"
#include "Core/CoreDefs.h"

using namespace DirectX;

namespace Dive
{
	namespace MeshFactory
	{
		// tangent 값이 부정확하다.
		DvMesh* CreateTriangle(float size)
		{
			// vertex buffer
			std::vector<VertexStatic> vertices;
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(0.0f, size / 2.0f, 0.0f), XMFLOAT2(0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			
			auto pVertexBuffer = Dive::DvVertexBuffer::Create(vertices.data(), sizeof(VertexStatic), (uint32_t)vertices.size());

			// index buffer
			std::vector<uint16_t> indices;
			indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);

			auto pIndexBuffer = Dive::DvIndexBuffer::Create(indices.data(), (uint32_t)indices.size());

			return DvMesh::Create(pVertexBuffer, pIndexBuffer);
		}

		DvMesh* CreateQuad()
		{
			return nullptr;
		}

		// tangent 값이 부정확하다.
		DvMesh* CreateCube(float size)
		{
			// vertex buffer
			std::vector<VertexStatic> vertices;
			
			// 전면
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
			
			// 후면
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));

			// 위
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

			// 아래
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
			
			// 왼쪽
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(-size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f));
			
			// 오른쪽
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, -size / 2.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, -size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
			vertices.emplace_back(XMFLOAT3(size / 2.0f, size / 2.0f, size / 2.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));

			auto pVertexBuffer = Dive::DvVertexBuffer::Create(vertices.data(), sizeof(VertexStatic), (uint32_t)vertices.size());
			
			// index buffer
			std::vector<uint16_t> indices;

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
			
			auto pIndexBuffer = Dive::DvIndexBuffer::Create(indices.data(), (uint32_t)indices.size());
			
			return DvMesh::Create(pVertexBuffer, pIndexBuffer);
		}

		DvMesh* CreatePlane(float length, float depth)
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

			auto pVertexBuffer = Dive::DvVertexBuffer::Create(vertices.data(), sizeof(VertexStatic), (uint32_t)vertices.size());

			std::vector<uint32_t> indices;
			indices.resize(6);

			indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
			indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

			auto pIndexBuffer = Dive::DvIndexBuffer::Create(indices.data(), (uint32_t)indices.size());

			return DvMesh::Create(pVertexBuffer, pIndexBuffer);
		}

		DvMesh* CreateSphere(float radius, int slices, int stacks)
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
					auto n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&pos));
					DirectX::XMStoreFloat3(&nor, n);
					tan = { -radius * sinf(phi) * sinf(theta), 0.0f, sinf(phi) * cosf(theta) };
					auto t = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&tan));
					DirectX::XMStoreFloat3(&tan, t);

					vertices.emplace_back(pos, uv, nor, tan);
				}
			}

			// 아래쪽 꼭지점
			pos = { 0.0f, -radius, 0.0f };
			uv = { 0.0f, 1.0f };
			nor = { 0.0f, -1.0f, 0.0f };
			tan = { 1.0f, 0.0f, 0.0f };
			vertices.emplace_back(pos, uv, nor, tan);

			auto pVertexBuffer = Dive::DvVertexBuffer::Create(vertices.data(), sizeof(VertexStatic), (uint32_t)vertices.size());

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

			auto pIndexBuffer = Dive::DvIndexBuffer::Create(indices.data(), (uint32_t)indices.size());

			return DvMesh::Create(pVertexBuffer, pIndexBuffer);
		}
	}
}