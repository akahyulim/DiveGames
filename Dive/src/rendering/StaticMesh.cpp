#include "stdafx.h"
#include "StaticMesh.h"
#include "graphics/Buffer.h"
#include "graphics/ShaderManager.h"
#include "graphics/Graphics.h"
#include "math/BoundingBox.h"

namespace Dive
{
	StaticMesh::StaticMesh()
		: m_type(eMeshType::Static)
	{
	}

	StaticMesh::~StaticMesh()
	{
		Clear();
	}

	bool StaticMesh::CreateBuffers()
	{
		// 정점 버퍼 생성
		{
			if (m_vertices.empty())
			{
				DV_LOG(StaticMesh, err, "[::CreateBuffers] 정점이 존재하지 않아 실패");
				return false;
			}

			m_vertexBuffer.reset();
			m_vertexBuffer = std::make_shared<Buffer>(
				eBufferType::VertexBuffer,
				m_vertices.data(),
				static_cast<uint32_t>(sizeof(StaticVertex)),
				static_cast<uint32_t>(m_vertices.size()));
			if (!m_vertexBuffer)
			{
				DV_LOG(StaticMesh, err, "[::CreateBuffers] VertexBuffer 생성 실패");
				m_vertexBuffer.reset();
				return false;
			}
		}

		// 인덱스 버퍼 생성
		if(!m_indices.empty())
		{
			m_indexBuffer.reset();
			m_indexBuffer = std::make_shared<Buffer>(
				eBufferType::IndexBuffer,
				m_indices.data(),
				static_cast<uint32_t>(sizeof(uint32_t)),
				static_cast<uint32_t>(m_indices.size()));
			if (!m_indexBuffer)
			{
				DV_LOG(StaticMesh, err, "[::CreateBuffers] IndexBuffer 생성 실패");
				return false;
			}
		}

		return true;
	}

	void StaticMesh::Clear()
	{
		m_boundingBox.reset();
		m_indexBuffer.reset();
		m_vertexBuffer.reset();

		m_indices.clear();
		m_indices.shrink_to_fit();

		m_vertices.clear();
		m_vertices.shrink_to_fit();
	}

	void StaticMesh::Draw()
	{
		assert(m_vertexBuffer);

		Graphics::GetDeviceContext()->IASetPrimitiveTopology(m_primitiveTopology);

		m_vertexBuffer->Bind();

		if (m_indexBuffer)
		{
			m_indexBuffer->Bind();
			Graphics::GetDeviceContext()->DrawIndexed(m_indexBuffer->GetCount(), 0, 0);
		}
		else
			Graphics::GetDeviceContext()->Draw(m_vertexBuffer->GetCount(), 0);
	}

	void StaticMesh::ComputeBouingBox()
	{
		if (m_vertices.empty())
		{
			DV_LOG(StaticMesh, err, "[::ComputeBouingBox] 빈 정점 정보");
			return;
		}

		m_boundingBox = std::make_unique<BoundingBox>(m_vertices);
	}

	void StaticMesh::AddVertices(const std::vector<StaticVertex>& vertices, uint32_t* outOffset)
	{
		// 현재 저장되어 있는 개수 리턴
		if (outOffset)
			*outOffset = static_cast<uint32_t>(m_vertices.size());

		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
	}

	void StaticMesh::AddIndices(const std::vector<uint32_t>& indices, uint32_t* outOffset)
	{
		if (outOffset)
			*outOffset = static_cast<uint32_t>(m_indices.size());

		m_indices.insert(m_indices.end(), indices.begin(), indices.end());
	}

	bool StaticMesh::LoadFromFile(const std::filesystem::path& filepath)
	{
		std::ifstream fin(filepath, std::ios::binary);
		if (!fin.is_open())
		{
			DV_LOG(StaticMesh, err, "[::LoadFromFile] 파일 열기 실패: {}", filepath.string());
			return false;
		}

		// 🔹 헤더 읽기
		char magic[4];
		uint32_t version = 0;
		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		fin.read(magic, sizeof(magic));
		if (std::strncmp(magic, "MESH", 4) != 0)
		{
			DV_LOG(StaticMesh, err, "[::LoadFromFile] Magic 헤더 불일치");
			return false;
		}

		fin.read(reinterpret_cast<char*>(&version), sizeof(version));
		fin.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
		fin.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));

		// 🔹 버텍스 버퍼 읽기
		m_vertices.resize(vertexCount);
		for (uint32_t i = 0; i < vertexCount; ++i)
		{
			StaticVertex& v = m_vertices[i];
			fin.read(reinterpret_cast<char*>(&v.Position), sizeof(v.Position));
			fin.read(reinterpret_cast<char*>(&v.Normal), sizeof(v.Normal));
			fin.read(reinterpret_cast<char*>(&v.Tangent), sizeof(v.Tangent));
			fin.read(reinterpret_cast<char*>(&v.TexCoord), sizeof(v.TexCoord));
		}

		// 🔹 인덱스 버퍼 읽기
		m_indices.resize(indexCount);
		fin.read(reinterpret_cast<char*>(m_indices.data()), sizeof(uint32_t) * indexCount);
		fin.close();

		SetFilepath(filepath);

		DV_LOG(StaticMesh, info, "[::LoadFromFile] 메시 로딩 완료: {}", filepath.string());
		return true;
	}

	bool StaticMesh::SaveToFile(const std::filesystem::path& filepath)
	{
		std::ofstream fout(filepath, std::ios::binary);
		if (!fout.is_open())
		{
			DV_LOG(StaticMesh, err, "[::SaveToFile] 파일 열기 실패: {}", filepath.string());
			return false;
		}

		// 🔹 헤더 작성
		const char magic[4] = { 'M', 'E', 'S', 'H' };
		uint32_t version = 1;
		uint32_t vertexCount = static_cast<uint32_t>(m_vertices.size());
		uint32_t indexCount = static_cast<uint32_t>(m_indices.size());

		fout.write(magic, sizeof(magic));
		fout.write(reinterpret_cast<const char*>(&version), sizeof(version));
		fout.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
		fout.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));

		// 🔹 버텍스 버퍼 저장
		for (const StaticVertex& v : m_vertices)
		{
			fout.write(reinterpret_cast<const char*>(&v.Position), sizeof(v.Position));
			fout.write(reinterpret_cast<const char*>(&v.Normal), sizeof(v.Normal));
			fout.write(reinterpret_cast<const char*>(&v.Tangent), sizeof(v.Tangent));
			fout.write(reinterpret_cast<const char*>(&v.TexCoord), sizeof(v.TexCoord));
		}

		// 🔹 인덱스 버퍼 저장
		fout.write(reinterpret_cast<const char*>(m_indices.data()), sizeof(uint32_t) * indexCount);
		fout.close();

		DV_LOG(StaticMesh, info, "[::SaveToFile] 메시 저장 완료: {}", filepath.string());
		return true;
	}
}
