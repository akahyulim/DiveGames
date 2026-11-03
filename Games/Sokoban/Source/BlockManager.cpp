#include "BlockManager.h"
#include "Graphics/Graphics.h"
#include "Graphics/Geometry.h"

using namespace DirectX;

namespace Sokoban
{
	constexpr float GRID_SIZE = 1.0f;

	constexpr XMFLOAT4 WALL_COLOR = { 0.2f, 0.2f, 0.2f, 1.0f };
	constexpr XMFLOAT4 FLOOR_COLOR = { 0.1f, 0.1f, 0.1f, 1.0f };
	constexpr XMFLOAT4 GOAL_COLOR = { 1.0f, 0.9f, 0.0f, 1.0f };
	constexpr XMFLOAT4 CRATE_COLOR = { 1.0f, 0.6f, 0.0f, 1.0f };
	constexpr XMFLOAT4 PLAYER_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };

	constexpr XMFLOAT3 WALL_SIZE = { GRID_SIZE, 0.5f, GRID_SIZE };
	constexpr XMFLOAT3 CRATE_SIZE = { GRID_SIZE, GRID_SIZE, GRID_SIZE };

	void BlockManager::Initialize(Dive::Graphics* graphics)
	{
		m_blocks.emplace(eBlockType::Wall, createCube(graphics, WALL_SIZE, WALL_COLOR));
		m_blocks.emplace(eBlockType::Floor, createFloor(graphics, FLOOR_COLOR));
		m_blocks.emplace(eBlockType::Goal, createFloor(graphics, GOAL_COLOR));
		m_blocks.emplace(eBlockType::Crate, createCube(graphics, CRATE_SIZE, CRATE_COLOR));
        m_blocks.emplace(eBlockType::Player, createPlayer(graphics, PLAYER_COLOR));
	}

	const Dive::Mesh* BlockManager::GetBlock(eBlockType type) const
	{
		auto it = m_blocks.find(type);
		return it != m_blocks.end() ? it->second.get() : nullptr;
	}

    std::unique_ptr<Dive::Mesh> BlockManager::createCube(Dive::Graphics* graphics, const DirectX::XMFLOAT3& size, const DirectX::XMFLOAT4& color)
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

    std::unique_ptr<Dive::Mesh> BlockManager::createFloor(Dive::Graphics* graphics, const DirectX::XMFLOAT4& color)
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

    std::unique_ptr<Dive::Mesh> BlockManager::createPlayer(Dive::Graphics* graphics, const DirectX::XMFLOAT4& color)
    {
        constexpr int latitudeBands = 30;
        constexpr int longitudeBands = 30;
        constexpr float radius = 0.5f;
        constexpr float height = 1.0f;

        XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);
        XMFLOAT3 centerTop(0.0f, height / 2.0f, 0.0f);
        XMFLOAT3 centerBottom(0.0f, -height / 2.0f, 0.0f);

        std::vector<Dive::ColorVertex> vertices;

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

                float u = lon / static_cast<float>(longitudeBands);
                float v = lat / static_cast<float>(latitudeBands) * 0.25f;

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

                float u = lon / static_cast<float>(longitudeBands);
                float v = 0.25f + yStep * 0.5f;

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

                float u = lon / static_cast<float>(longitudeBands);
                float v = 0.75f + lat / static_cast<float>(latitudeBands) * 0.25f;

                XMVECTOR posVec = XMLoadFloat3(&position);
                XMVECTOR centerVec = XMLoadFloat3(&centerBottom);
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

        std::vector<uint32_t> indices;

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

        indices.reserve((latitudeBands * 2 + 1) * longitudeBands * 6);

        // 인덱스 연결
        int topStart = 0;
        int cylinderStart = topStart + (latitudeBands + 1) * (longitudeBands + 1);
        int bottomStart = cylinderStart + 2 * (longitudeBands + 1);

        connectBands(topStart, latitudeBands);               // 위쪽 반구
        connectBands(cylinderStart, 1);                      // 실린더
        connectBandsReversed(bottomStart, latitudeBands);    // 아래쪽 반구 (반전)

        return std::make_unique<Dive::Mesh>(graphics, vertices, indices);
    }
    /*
    std::unique_ptr<Dive::Mesh> BlockManager::createPlayer(Dive::Graphics* graphics, const DirectX::XMFLOAT4& color)
    {
        std::vector<Dive::ColorVertex> vertices;
        std::vector<uint32_t> indices;

        float radius = 0.3f;          // 몸통 및 돔의 반지름
        float cylinderHeight = 0.5f;  // 원통 몸통의 높이
        int sliceCount = 16;          // 가로 분할 수 (원형 디테일)
        int cylinderStack = 2;        // 원통의 세로 분할 수
        int domeStack = 4;            // 윗면 돔(반구)의 세로 분할 수

        int verticesPerRing = sliceCount + 1;

        // -------------------------------------------------------------------------
        // 1. 아랫면 평평한 바닥 (Bottom Flat Disk) - y: 0.0
        // -------------------------------------------------------------------------
        uint32_t bottomCenterIndex = static_cast<uint32_t>(vertices.size());
        Dive::ColorVertex bottomCenter;
        bottomCenter.Position = { 0.0f, 0.0f, 0.0f };
        bottomCenter.Color = color;
        bottomCenter.Normal = { 0.0f, -1.0f, 0.0f };
        bottomCenter.Tangent = { 1.0f, 0.0f, 0.0f };
        bottomCenter.BiNormal = { 0.0f, 0.0f, 1.0f };
        vertices.push_back(bottomCenter);

        uint32_t bottomRingStartIndex = static_cast<uint32_t>(vertices.size());
        for (int j = 0; j <= sliceCount; ++j)
        {
            float theta = 2.0f * DirectX::XM_PI * (static_cast<float>(j) / sliceCount);
            float x = radius * std::cos(theta);
            float z = radius * std::sin(theta);

            Dive::ColorVertex v;
            v.Position = { x, 0.0f, z };
            v.Color = color;
            v.Normal = { 0.0f, -1.0f, 0.0f };
            v.Tangent = { -std::sin(theta), 0.0f, std::cos(theta) };
            v.BiNormal = { 0.0f, 0.0f, 1.0f };
            vertices.push_back(v);
        }

        // 바닥 인덱스 생성 (삼각형 부채꼴)
        for (int j = 0; j < sliceCount; ++j)
        {
            uint32_t current = bottomRingStartIndex + j;
            uint32_t next = bottomRingStartIndex + (j + 1);

            //indices.push_back(bottomCenterIndex);
            //indices.push_back(next);
            //indices.push_back(current);
            indices.push_back(bottomCenterIndex);
            indices.push_back(current);
            indices.push_back(next);
        }

        // -------------------------------------------------------------------------
        // 2. 원통 몸통 (Cylinder Body) - y: 0.0 ~ cylinderHeight
        // -------------------------------------------------------------------------
        uint32_t cylinderStartVertexIndex = static_cast<uint32_t>(vertices.size());

        for (int i = 0; i <= cylinderStack; ++i)
        {
            float t = static_cast<float>(i) / cylinderStack;
            float y = t * cylinderHeight;

            for (int j = 0; j <= sliceCount; ++j)
            {
                float theta = 2.0f * DirectX::XM_PI * (static_cast<float>(j) / sliceCount);
                float x = radius * std::cos(theta);
                float z = radius * std::sin(theta);

                Dive::ColorVertex v;
                v.Position = { x, y, z };
                v.Color = color;
                v.Normal = { x / radius, 0.0f, z / radius }; // 옆면 노멀
                v.Tangent = { -std::sin(theta), 0.0f, std::cos(theta) };
                v.BiNormal = { 0.0f, 1.0f, 0.0f };
                vertices.push_back(v);
            }
        }

        // 원통 몸통 인덱스 생성
        for (int i = 0; i < cylinderStack; ++i)
        {
            for (int j = 0; j < sliceCount; ++j)
            {
                uint32_t row1 = cylinderStartVertexIndex + i * verticesPerRing;
                uint32_t row2 = cylinderStartVertexIndex + (i + 1) * verticesPerRing;

                uint32_t idx0 = row1 + j;
                uint32_t idx1 = row2 + j;
                uint32_t idx2 = row1 + (j + 1);
                uint32_t idx3 = row2 + (j + 1);

                indices.push_back(idx0);
                indices.push_back(idx2);
                indices.push_back(idx1);

                indices.push_back(idx1);
                indices.push_back(idx2);
                indices.push_back(idx3);
            }
        }

        // -------------------------------------------------------------------------
        // 3. 윗면 돔 반구 (Top Dome Hemisphere) - y: cylinderHeight ~ cylinderHeight + radius
        // -------------------------------------------------------------------------
        uint32_t domeStartVertexIndex = static_cast<uint32_t>(vertices.size());

        for (int i = 0; i <= domeStack; ++i)
        {
            float t = static_cast<float>(i) / domeStack;
            float phi = t * DirectX::XM_PIDIV2; // 0 ~ pi/2
            float y = cylinderHeight + radius * std::sin(phi);
            float ringRadius = radius * std::cos(phi);

            for (int j = 0; j <= sliceCount; ++j)
            {
                float theta = 2.0f * DirectX::XM_PI * (static_cast<float>(j) / sliceCount);
                float x = ringRadius * std::cos(theta);
                float z = ringRadius * std::sin(theta);

                DirectX::XMVECTOR normalVec = DirectX::XMVector3Normalize(DirectX::XMVectorSet(x, radius * std::sin(phi), z, 0.0f));
                DirectX::XMFLOAT3 n;
                DirectX::XMStoreFloat3(&n, normalVec);

                Dive::ColorVertex v;
                v.Position = { x, y, z };
                v.Color = color;
                v.Normal = n;
                v.Tangent = { -std::sin(theta), 0.0f, std::cos(theta) };
                v.BiNormal = { 0.0f, 1.0f, 0.0f };
                vertices.push_back(v);
            }
        }

        // 윗면 돔 인덱스 생성
        for (int i = 0; i < domeStack; ++i)
        {
            for (int j = 0; j < sliceCount; ++j)
            {
                uint32_t row1 = domeStartVertexIndex + i * verticesPerRing;
                uint32_t row2 = domeStartVertexIndex + (i + 1) * verticesPerRing;

                uint32_t idx0 = row1 + j;
                uint32_t idx1 = row2 + j;
                uint32_t idx2 = row1 + (j + 1);
                uint32_t idx3 = row2 + (j + 1);

                indices.push_back(idx0);
                indices.push_back(idx2);
                indices.push_back(idx1);

                indices.push_back(idx1);
                indices.push_back(idx2);
                indices.push_back(idx3);
            }
        }

        // -------------------------------------------------------------------------
        // 4. Mesh 객체 생성 및 반환
        // -------------------------------------------------------------------------
        return std::make_unique<Dive::Mesh>(graphics, vertices, indices);
    }
    */
}