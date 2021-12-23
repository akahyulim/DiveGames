#pragma once
#include "Src/Renderer/dvMesh.h"

using namespace DirectX;

namespace DiveEditor
{
	namespace Utility
	{
		// Cube
		inline void CreateCube(std::vector<DiveEngine::Vertex_StaticMesh>& vertices, std::vector<uint32_t>& indices)
		{
			// vertices
			{
				// front
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, -0.5f, -0.5f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, 0.5f, -0.5f),	XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, 0.5f, -0.5f),	XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, -0.5f, -0.5f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));

				// back
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, -0.5f, 0.5f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, 0.5f, 0.5f),		XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, 0.5f, 0.5f),	XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, -0.5f, 0.5f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				
				// top
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, 0.5f, -0.5f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, 0.5f, 0.5f),	XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, 0.5f, 0.5f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, 0.5f, -0.5f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));

				// bottom
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, -0.5f, 0.5f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, -0.5f, -0.5f),	XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, -0.5f, -0.5f),	XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, -0.5f, 0.5f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));

				// left
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, -0.5f, 0.5f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, 0.5f, 0.5f),	XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, 0.5f, -0.5f),	XMFLOAT2(1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, -0.5f, -0.5f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));

				// right
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, -0.5f, -0.5f),	XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, 0.5f, -0.5f),	XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, 0.5f, 0.5f),		XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, -0.5f, 0.5f),	XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
			}

			// indices
			{
				// front
				indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);
				indices.emplace_back(0);	indices.emplace_back(2);	indices.emplace_back(3);

				// back
				indices.emplace_back(4);	indices.emplace_back(5);	indices.emplace_back(6);
				indices.emplace_back(4);	indices.emplace_back(6);	indices.emplace_back(7);

				// top
				indices.emplace_back(8);	indices.emplace_back(9);	indices.emplace_back(10);
				indices.emplace_back(8);	indices.emplace_back(10);	indices.emplace_back(11);

				// bottom
				indices.emplace_back(12);	indices.emplace_back(13);	indices.emplace_back(14);
				indices.emplace_back(12);	indices.emplace_back(14);	indices.emplace_back(15);

				// left
				indices.emplace_back(16);	indices.emplace_back(17);	indices.emplace_back(18);
				indices.emplace_back(16);	indices.emplace_back(18);	indices.emplace_back(19);

				// right
				indices.emplace_back(20);	indices.emplace_back(21);	indices.emplace_back(22);
				indices.emplace_back(20);	indices.emplace_back(22);	indices.emplace_back(23);
			}
		}

		// Sphere: UV Sphere
		inline void CreateSphere(std::vector<DiveEngine::Vertex_StaticMesh>& vertices, std::vector<uint32_t>& indices, float radius = 0.5f, int slices = 20, int stacks = 20)
		{
			// vertices
			{
				// top vertex
				vertices.emplace_back(XMFLOAT3(0.0f, radius, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));

				// generate vertices
				const auto phiStep		= XM_PI / static_cast<float>(stacks);
				const auto thetaStep	= 2.0f * XM_PI / static_cast<float>(slices);
				
				for (int i = 1; i <= stacks - 1; i++)
				{
					auto phi = static_cast<float>(i) * phiStep;

					for (int j = 0; j <= slices; j++)
					{
						auto theta = static_cast<float>(j) * thetaStep;

						auto pos = XMFLOAT3(
							radius * std::sin(phi) * std::cos(theta),
							radius * std::cos(phi),
							radius * std::sin(phi) * std::sin(theta)
						);
						auto uv = XMFLOAT2(theta / (XM_PI * 2), phi / XM_PI);
						XMFLOAT3 nor;
						XMStoreFloat3(&nor, XMVector3Normalize(XMLoadFloat3(&pos)));
						auto tan = XMFLOAT3(-radius * std::sin(phi) * std::sin(theta), 0.0f, radius * std::sin(phi) * std::cos(theta));
						XMStoreFloat3(&tan, XMVector3Normalize(XMLoadFloat3(&tan)));

						vertices.emplace_back(pos, uv, nor, tan);
					}
				}

				// bottom vertex
				vertices.emplace_back(XMFLOAT3(0.0f, -radius, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
			}

			// indices
			{
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
				
				int southPoleIndex = (int)vertices.size() - 1;
				baseIndex = southPoleIndex - ringVertexCount;
				
				for (int i = 0; i < slices; i++)
				{
					indices.emplace_back(southPoleIndex);
					indices.emplace_back(baseIndex + i);
					indices.emplace_back(baseIndex + i + 1);
				}
			}
		}

		// Capsule

		// Cylinder
		inline void CreateCylinder(std::vector<DiveEngine::Vertex_StaticMesh>& vertices, std::vector<uint32_t>& indices, float radiusTop = 0.5f, float radiusBottom = 0.5f, float height = 1.0f, int slices = 15, int stacks = 15)
		{
			const float stackHeight = height / stacks;
			const float radiusStep = (radiusTop - radiusBottom) / stacks;
			const float ringCount = (float)(stacks + 1);

			for (int i = 0; i < ringCount; i++)
			{
				const float y = -0.5f * height + i * stackHeight;
				const float r = radiusBottom + i * radiusStep;
				const float dTheta = 2.0f * XM_PI / slices;
				for (int j = 0; j <= slices; j++)
				{
					const float c = cos(j * dTheta);
					const float s = sin(j * dTheta);

					auto pos = XMFLOAT3(r * c, y, r * s);
					auto uv = XMFLOAT2((float)j / slices, 1.0f - (float)i / stacks);
					auto tan = XMFLOAT3(-s, 0.0f, c);

					const float dr = radiusBottom - radiusTop;
					auto bitangent = XMFLOAT3(dr * c, -height, dr * s);

					XMFLOAT3 nor;
					XMStoreFloat3(&nor, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&tan), XMLoadFloat3(&bitangent))));

					vertices.emplace_back(pos, uv, nor, tan);

				}
			}

			const int ringVertexCount = slices + 1;
			for (int i = 0; i < stacks; i++)
			{
				for (int j = 0; j < slices; j++)
				{
					indices.push_back(i * ringVertexCount + j);
					indices.push_back((i + 1) * ringVertexCount + j);
					indices.push_back((i + 1) * ringVertexCount + j + 1);

					indices.push_back(i * ringVertexCount + j);
					indices.push_back((i + 1) * ringVertexCount + j + 1);
					indices.push_back(i * ringVertexCount + j + 1);
				}
			}

			// Build top cap
			int baseIndex = (int)vertices.size();
			float y = 0.5f * height;
			const float dTheta = 2.0f * XM_PI / slices;

			XMFLOAT3 nor;
			XMFLOAT3 tan;

			for (int i = 0; i <= slices; i++)
			{
				const float x = radiusTop * cos(i * dTheta);
				const float z = radiusTop * sin(i * dTheta);
				const float u = x / height + 0.5f;
				const float v = z / height + 0.5f;

				nor = XMFLOAT3(0, 1, 0);
				tan = XMFLOAT3(1, 0, 0);
				vertices.emplace_back(XMFLOAT3(x, y, z), XMFLOAT2(u, v), nor, tan);
			}

			nor = XMFLOAT3(0, 1, 0);
			tan = XMFLOAT3(1, 0, 0);
			vertices.emplace_back(XMFLOAT3(0, y, 0), XMFLOAT2(0.5f, 0.5f), nor, tan);

			int centerIndex = (int)vertices.size() - 1;
			for (int i = 0; i < slices; i++)
			{
				indices.push_back(centerIndex);
				indices.push_back(baseIndex + i + 1);
				indices.push_back(baseIndex + i);
			}

			// Build bottom cap
			baseIndex = (int)vertices.size();
			y = -0.5f * height;

			for (int i = 0; i <= slices; i++)
			{
				const float x = radiusBottom * cos(i * dTheta);
				const float z = radiusBottom * sin(i * dTheta);
				const float u = x / height + 0.5f;
				const float v = z / height + 0.5f;

				nor = XMFLOAT3(0, -1, 0);
				tan = XMFLOAT3(1, 0, 0);
				vertices.emplace_back(XMFLOAT3(x, y, z), XMFLOAT2(u, v), nor, tan);
			}

			nor = XMFLOAT3(0, -1, 0);
			tan = XMFLOAT3(1, 0, 0);
			vertices.emplace_back(XMFLOAT3(0, y, 0), XMFLOAT2(0.5f, 0.5f), nor, tan);

			centerIndex = (int)vertices.size() - 1;
			for (int i = 0; i < slices; i++)
			{
				indices.push_back(centerIndex);
				indices.push_back(baseIndex + i);
				indices.push_back(baseIndex + i + 1);
			}
		}

		// Plane
		inline void CreatePlane(std::vector<DiveEngine::Vertex_StaticMesh>& vertices, std::vector<uint32_t>& indices)
		{
			// vertices
			{
				if (!vertices.empty())
				{
					vertices.clear();
					vertices.shrink_to_fit();
				}

				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, 0.0f, -0.5f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, 0.0f, 0.5f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, 0.0f, 0.5f),  XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, 0.0f, -0.5f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)));
			}

			// indices
			{
				if (!indices.empty())
				{
					indices.clear();
					indices.shrink_to_fit();
				}

				indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);
				indices.emplace_back(0);	indices.emplace_back(2);	indices.emplace_back(3);
			}
		}

		// Quad
		inline void CreateQuad(std::vector<DiveEngine::Vertex_StaticMesh>& vertices, std::vector<uint32_t>& indices)
		{
			// vertices
			{
				if (!vertices.empty())
				{
					vertices.clear();
					vertices.shrink_to_fit();
				}

				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
				vertices.emplace_back(DiveEngine::Vertex_StaticMesh(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)));
			}

			// indices
			{
				if (!indices.empty())
				{
					indices.clear();
					indices.shrink_to_fit();
				}

				indices.emplace_back(0);	indices.emplace_back(1);	indices.emplace_back(2);
				indices.emplace_back(0);	indices.emplace_back(2);	indices.emplace_back(3);
			}
		}
	}
}