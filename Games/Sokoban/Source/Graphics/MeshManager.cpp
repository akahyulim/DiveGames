#include "MeshManager.h"
#include "Graphics/Graphics.h"
#include "Graphics/Geometry.h"
#include "../Stage.h"

using namespace DirectX;

namespace Sokoban
{
	constexpr float GRID_SIZE = 1.0f;

	constexpr XMFLOAT4 WALL_COLOR = { 0.2f, 0.2f, 0.2f, 1.0f };
	constexpr XMFLOAT4 FLOOR_COLOR = { 0.1f, 0.1f, 0.1f, 1.0f };
	constexpr XMFLOAT4 GOAL_COLOR = { 1.0f, 0.9f, 0.0f, 1.0f };
	constexpr XMFLOAT4 CRATE_COLOR = { 1.0f, 0.6f, 0.0f, 1.0f };
	constexpr XMFLOAT4 PLAYER_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };

	constexpr XMFLOAT3 WALL_SIZE = { GRID_SIZE, 3.0f, GRID_SIZE };
	constexpr XMFLOAT3 CRATE_SIZE = { GRID_SIZE, GRID_SIZE, GRID_SIZE };

	void MeshManager::Initialize(Dive::Graphics* graphics)
	{
		m_wall = createCube(graphics, WALL_SIZE, WALL_COLOR);
		m_floor = createFloor(graphics, FLOOR_COLOR);
        m_goal = createFloor(graphics, GOAL_COLOR);
		m_crate = createCube(graphics, CRATE_SIZE, CRATE_COLOR);
        m_player = createPlayer(graphics, PLAYER_COLOR);
	}

    const Dive::Mesh* MeshManager::GetTile(uint8_t type) const
    {
        switch (type)
        {
        case TileType::Wall:  return m_wall.get();
        case TileType::Floor: return m_floor.get();
        case TileType::Goal:  return m_goal.get();
        default:               return nullptr;
        }
    }

    std::unique_ptr<Dive::Mesh> MeshManager::createCube(Dive::Graphics* graphics, const DirectX::XMFLOAT3& size, const DirectX::XMFLOAT4& color)
    {
        std::vector<Dive::ColorVertex> vertices;
        vertices.reserve(24);

        // 전면
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, 0.0f, -size.z / 2.0f), color, XMFLOAT3(0, 0, -1), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, size.y, -size.z / 2.0f), color, XMFLOAT3(0, 0, -1), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, 0.0f, -size.z / 2.0f), color, XMFLOAT3(0, 0, -1), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, size.y, -size.z / 2.0f), color, XMFLOAT3(0, 0, -1), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, -1.0f, 0));

        // 후면
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, 0.0f, size.z / 2.0f), color, XMFLOAT3(0, 0, 1), XMFLOAT3(-1.0f, 0, 0), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, size.y, size.z / 2.0f), color, XMFLOAT3(0, 0, 1), XMFLOAT3(-1.0f, 0, 0), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, 0.0f, size.z / 2.0f), color, XMFLOAT3(0, 0, 1), XMFLOAT3(-1.0f, 0, 0), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, size.y, size.z / 2.0f), color, XMFLOAT3(0, 0, 1), XMFLOAT3(-1.0f, 0, 0), XMFLOAT3(0, -1.0f, 0));

        // 위
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, size.y, -size.z / 2.0f), color, XMFLOAT3(0, 1.0f, 0), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, -1));
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, size.y, size.z / 2.0f), color, XMFLOAT3(0, 1.0f, 0), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, -1));
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, size.y, -size.z / 2.0f), color, XMFLOAT3(0, 1.0f, 0), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, -1));
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, size.y, size.z / 2.0f), color, XMFLOAT3(0, 1.0f, 0), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, -1));

        // 아래
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, 0.0f, size.z / 2.0f), color, XMFLOAT3(0, -1.0f, 0), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, 1));
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, 0.0f, -size.z / 2.0f), color, XMFLOAT3(0, -1.0f, 0), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, 1));
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, 0.0f, size.z / 2.0f), color, XMFLOAT3(0, -1.0f, 0), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, 1));
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, 0.0f, -size.z / 2.0f), color, XMFLOAT3(0, -1.0f, 0), XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, 1));

        // 왼쪽
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, 0.0f, size.z / 2.0f), color, XMFLOAT3(-1.0f, 0, 0), XMFLOAT3(0, 0, -1), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, size.y, size.z / 2.0f), color, XMFLOAT3(-1.0f, 0, 0), XMFLOAT3(0, 0, -1), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, 0.0f, -size.z / 2.0f), color, XMFLOAT3(-1.0f, 0, 0), XMFLOAT3(0, 0, -1), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(-size.x / 2.0f, size.y, -size.z / 2.0f), color, XMFLOAT3(-1.0f, 0, 0), XMFLOAT3(0, 0, -1), XMFLOAT3(0, -1.0f, 0));

        // 오른쪽
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, 0.0f, -size.z / 2.0f), color, XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, size.y, -size.z / 2.0f), color, XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, 0.0f, size.z / 2.0f), color, XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(0, -1.0f, 0));
        vertices.emplace_back(XMFLOAT3(size.x / 2.0f, size.y, size.z / 2.0f), color, XMFLOAT3(1.0f, 0, 0), XMFLOAT3(0, 0, 1), XMFLOAT3(0, -1.0f, 0));

        std::vector<uint32_t> indices;
        indices.reserve(36);

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

        return std::make_unique<Dive::Mesh>(graphics, vertices, indices);
    }

    std::unique_ptr<Dive::Mesh> MeshManager::createFloor(Dive::Graphics* graphics, const DirectX::XMFLOAT4& color)
    {
        std::vector<Dive::ColorVertex> vertices;
        vertices.resize(4);

        // X-Z 평면 바닥 (Y축은 0.0f로 고정, 노멀은 위쪽(0, 1, 0)을 향하도록 설정)

        // 좌측 상단 (X: -0.5, Z: +0.5)
        vertices[0] = {
            XMFLOAT3(-1.0f / 2.0f, 0.0f, 1.0f / 2.0f),
            color,
            XMFLOAT3(0.0f, 1.0f, 0.0f), // 법선 벡터: 위쪽
            XMFLOAT3(1.0f, 0.0f, 0.0f),
            XMFLOAT3(0.0f, 0.0f, -1.0f)
        };

        // 우측 상단 (X: +0.5, Z: +0.5)
        vertices[1] = {
            XMFLOAT3(1.0f / 2.0f, 0.0f, 1.0f / 2.0f),
            color,
            XMFLOAT3(0.0f, 1.0f, 0.0f),
            XMFLOAT3(1.0f, 0.0f, 0.0f),
            XMFLOAT3(0.0f, 0.0f, -1.0f)
        };

        // 좌측 하단 (X: -0.5, Z: -0.5)
        vertices[2] = {
            XMFLOAT3(-1.0f / 2.0f, 0.0f, -1.0f / 2.0f),
            color,
            XMFLOAT3(0.0f, 1.0f, 0.0f),
            XMFLOAT3(1.0f, 0.0f, 0.0f),
            XMFLOAT3(0.0f, 0.0f, -1.0f)
        };

        // 우측 하단 (X: +0.5, Z: -0.5)
        vertices[3] = {
            XMFLOAT3(1.0f / 2.0f, 0.0f, -1.0f / 2.0f),
            color,
            XMFLOAT3(0.0f, 1.0f, 0.0f),
            XMFLOAT3(1.0f, 0.0f, 0.0f),
            XMFLOAT3(0.0f, 0.0f, -1.0f)
        };

        std::vector<uint32_t> indices;
        indices.resize(6);

        // 윈딩 오더(Clockwise/Counter-Clockwise)에 맞춰 인덱스 재정렬
        indices[0] = 0; indices[1] = 1; indices[2] = 2;
        indices[3] = 2; indices[4] = 1; indices[5] = 3;

        return std::make_unique<Dive::Mesh>(graphics, vertices, indices);
    }

    std::unique_ptr<Dive::Mesh> MeshManager::createPlayer(Dive::Graphics* graphics, const DirectX::XMFLOAT4& color)
    {
        constexpr int latitudeBands = 30;
        constexpr int longitudeBands = 30;
        constexpr float radius = 0.3f;
        constexpr float height = 0.5f;

        XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);
        XMFLOAT3 centerTop(0.0f, height, 0.0f);

        std::vector<Dive::ColorVertex> vertices;

        // 1. 위쪽 반구
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

                XMVECTOR binormalVec = XMVector3Normalize(XMVector3Cross(normalVec, tangentVec));
                XMFLOAT3 binormal;
                XMStoreFloat3(&binormal, binormalVec);

                vertices.emplace_back(position, PLAYER_COLOR, normal, tangent, binormal);
            }
        }

        // 2. 실린더 몸통
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

                XMVECTOR binormalVec = XMVector3Normalize(XMVector3Cross(normalVec, tangentVec));
                XMFLOAT3 binormal;
                XMStoreFloat3(&binormal, binormalVec);

                vertices.emplace_back(position, PLAYER_COLOR, normal, tangent, binormal);
            }
        }

        // 3. 아랫면 평평한 바닥 (Bottom Flat Disk) - y = 0.0f
        uint32_t bottomCenterIndex = static_cast<uint32_t>(vertices.size());
        Dive::ColorVertex bottomCenter;
        bottomCenter.Position = { 0.0f, 0.0f, 0.0f };
        bottomCenter.Color = PLAYER_COLOR;
        bottomCenter.Normal = { 0.0f, -1.0f, 0.0f };
        bottomCenter.Tangent = { 1.0f, 0.0f, 0.0f };
        bottomCenter.BiNormal = { 0.0f, 0.0f, 1.0f };
        vertices.push_back(bottomCenter);

        uint32_t bottomRingStartIndex = static_cast<uint32_t>(vertices.size());
        for (int j = 0; j <= longitudeBands; ++j)
        {
            float phi = 2.0f * XM_PI * (static_cast<float>(j) / longitudeBands);
            float x = radius * std::cos(phi);
            float z = radius * std::sin(phi);

            Dive::ColorVertex v;
            v.Position = { x, 0.0f, z };
            v.Color = PLAYER_COLOR;
            v.Normal = { 0.0f, -1.0f, 0.0f };
            v.Tangent = { -std::sin(phi), 0.0f, std::cos(phi) };
            v.BiNormal = { 0.0f, 0.0f, 1.0f };
            vertices.push_back(v);
        }

        std::vector<uint32_t> indices;

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

        indices.reserve((latitudeBands * 2 + 1) * longitudeBands * 6);

        // 인덱스 연결
        int topStart = 0;
        int cylinderStart = topStart + (latitudeBands + 1) * (longitudeBands + 1);

        connectBands(topStart, latitudeBands); // 위쪽 반구
        connectBands(cylinderStart, 1);        // 실린더

        // 바닥 디스크 인덱스 생성 (삼각형 부채꼴)
        for (int j = 0; j < longitudeBands; ++j)
        {
            uint32_t current = bottomRingStartIndex + j;
            uint32_t next = bottomRingStartIndex + (j + 1);

            indices.push_back(bottomCenterIndex);
            indices.push_back(current);
            indices.push_back(next);
        }

        return std::make_unique<Dive::Mesh>(graphics, vertices, indices);
    }
}