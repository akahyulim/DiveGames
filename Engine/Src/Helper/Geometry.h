#pragma once
#include "../Renderer/Graphics/GraphicsInclude.h"
#include "../Renderer/VertexTypes.h"
#include <vector>

//========================================================//
// 일단 Triangle List로 보면 형태가 맞다.                 //
// 하지만 Line List로 보면 인덱스가 좀 이상하다.          //
//========================================================//
namespace Dive::utility::geometry
{
    static void CreatePlane(std::vector<VertexType_PosTexNorTan>& vertices, std::vector<unsigned int>& indices)
    {
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, 0.0f, -0.5f), DirectX::XMFLOAT2(0, 1), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT3(1, 0, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, 0.0f, 0.5f), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT3(1, 0, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, 0.0f, 0.5f), DirectX::XMFLOAT2(1, 0), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT3(1, 0, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, 0.0f, -0.5f), DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT3(1, 0, 0));

        indices.emplace_back(0);    indices.emplace_back(1);    indices.emplace_back(2);
        indices.emplace_back(0);    indices.emplace_back(2);    indices.emplace_back(3);
    }

	static void CreateCube(std::vector<VertexType_PosTexNorTan>& vertices, std::vector<unsigned int>& indices)
	{
        // front
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(0, 1), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(1, 0), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT3(0, 1, 0));

        // bottom
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(0, 1), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT3(1, 0, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT3(1, 0, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT3(1, 0, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(1, 0), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT3(1, 0, 0));

        // back
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(1, 0), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(0, 1), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT3(0, 1, 0));

        // top
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT3(1, 0, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(0, 1), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT3(1, 0, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(1, 0), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT3(1, 0, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT3(1, 0, 0));

        // left
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(0, 1), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(1, 0), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT3(0, 1, 0));

        // right
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(1, 0), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT2(0, 1), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT3(0, 1, 0));
        vertices.emplace_back(DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT3(0, 1, 0));

        // front
        indices.emplace_back(0); indices.emplace_back(1); indices.emplace_back(2);
        indices.emplace_back(2); indices.emplace_back(1); indices.emplace_back(3);

        // bottom
        indices.emplace_back(4); indices.emplace_back(5); indices.emplace_back(6);
        indices.emplace_back(6); indices.emplace_back(5); indices.emplace_back(7);

        // back
        indices.emplace_back(10); indices.emplace_back(9); indices.emplace_back(8);
        indices.emplace_back(11); indices.emplace_back(9); indices.emplace_back(10);

        // top
        indices.emplace_back(14); indices.emplace_back(13); indices.emplace_back(12);
        indices.emplace_back(15); indices.emplace_back(13); indices.emplace_back(14);

        // left
        indices.emplace_back(16); indices.emplace_back(17); indices.emplace_back(18);
        indices.emplace_back(18); indices.emplace_back(17); indices.emplace_back(19);

        // right
        indices.emplace_back(22); indices.emplace_back(21); indices.emplace_back(20);
        indices.emplace_back(23); indices.emplace_back(21); indices.emplace_back(22);
	}

    // Sphere
    static void CreateSphere(std::vector<VertexType_PosTexNorTan>& vertices, std::vector<unsigned int>& indices)
    {
    }

    // Capsule
    static void CreateCapsule(std::vector<VertexType_PosTexNorTan>& vertices, std::vector<unsigned int>& indices)
    {
    }

    // Cylinder
    static void CreateCylinder(std::vector<VertexType_PosTexNorTan>& vertices, std::vector<unsigned int>& indices)
    {
    }
}