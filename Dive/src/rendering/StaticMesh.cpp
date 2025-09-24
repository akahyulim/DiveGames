#include "stdafx.h"
#include "StaticMesh.h"
#include "graphics/Buffer.h"
#include "graphics/ShaderManager.h"
#include "graphics/Graphics.h"

namespace Dive
{
	StaticMesh::StaticMesh()
		: m_Type(eMeshType::Static)
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
			if (m_Vertices.empty())
			{
				DV_LOG(StaticMesh, err, "[::CreateBuffers] 정점이 존재하지 않아 실패");
				return false;
			}

			m_VertexBuffer.reset();
			m_VertexBuffer = std::make_shared<Buffer>(
				eBufferType::VertexBuffer,
				m_Vertices.data(),
				static_cast<uint32_t>(sizeof(StaticVertex)),
				static_cast<uint32_t>(m_Vertices.size()));
			if (!m_VertexBuffer)
			{
				DV_LOG(StaticMesh, err, "[::CreateBuffers] VertexBuffer 생성 실패");
				m_VertexBuffer.reset();
				return false;
			}

			m_LocalBoundingBox = BoundingBox(m_Vertices);
		}

		// 인덱스 버퍼 생성
		if(!m_Inidices.empty())
		{
			m_IndexBuffer.reset();
			m_IndexBuffer = std::make_shared<Buffer>(
				eBufferType::IndexBuffer,
				m_Inidices.data(),
				static_cast<uint32_t>(sizeof(uint32_t)),
				static_cast<uint32_t>(m_Inidices.size()));
			if (!m_IndexBuffer)
			{
				DV_LOG(StaticMesh, err, "[::CreateBuffers] IndexBuffer 생성 실패");
				return false;
			}
		}

		return true;
	}

	void StaticMesh::Clear()
	{
		m_IndexBuffer.reset();
		m_VertexBuffer.reset();

		m_Inidices.clear();
		m_Inidices.shrink_to_fit();

		m_Vertices.clear();
		m_Vertices.shrink_to_fit();
	}

	void StaticMesh::Draw()
	{
		assert(m_VertexBuffer);

		Graphics::GetDeviceContext()->IASetPrimitiveTopology(m_PrimitiveTopology);

		m_VertexBuffer->Bind();

		if (m_IndexBuffer)
		{
			m_IndexBuffer->Bind();
			Graphics::GetDeviceContext()->DrawIndexed(m_IndexBuffer->GetCount(), 0, 0);
		}
		else
			Graphics::GetDeviceContext()->Draw(m_VertexBuffer->GetCount(), 0);
	}

	void StaticMesh::AddVertices(const std::vector<StaticVertex>& vertices, uint32_t* outOffset)
	{
		// 현재 저장되어 있는 개수 리턴
		if (outOffset)
			*outOffset = static_cast<uint32_t>(m_Vertices.size());

		m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
	}

	void StaticMesh::AddIndices(const std::vector<uint32_t>& indices, uint32_t* outOffset)
	{
		if (outOffset)
			*outOffset = static_cast<uint32_t>(m_Inidices.size());

		m_Inidices.insert(m_Inidices.end(), indices.begin(), indices.end());
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
		m_Vertices.resize(vertexCount);
		for (uint32_t i = 0; i < vertexCount; ++i)
		{
			StaticVertex& v = m_Vertices[i];
			fin.read(reinterpret_cast<char*>(&v.Position), sizeof(v.Position));
			fin.read(reinterpret_cast<char*>(&v.TexCoord), sizeof(v.TexCoord));
			fin.read(reinterpret_cast<char*>(&v.Normal), sizeof(v.Normal));
			fin.read(reinterpret_cast<char*>(&v.Tangent), sizeof(v.Tangent));
			fin.read(reinterpret_cast<char*>(&v.BiNormal), sizeof(v.BiNormal));
		}

		// 🔹 인덱스 버퍼 읽기
		m_Inidices.resize(indexCount);
		fin.read(reinterpret_cast<char*>(m_Inidices.data()), sizeof(uint32_t) * indexCount);
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
		uint32_t vertexCount = static_cast<uint32_t>(m_Vertices.size());
		uint32_t indexCount = static_cast<uint32_t>(m_Inidices.size());

		fout.write(magic, sizeof(magic));
		fout.write(reinterpret_cast<const char*>(&version), sizeof(version));
		fout.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
		fout.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));

		// 🔹 버텍스 버퍼 저장
		for (const StaticVertex& v : m_Vertices)
		{
			fout.write(reinterpret_cast<const char*>(&v.Position), sizeof(v.Position));
			fout.write(reinterpret_cast<const char*>(&v.TexCoord), sizeof(v.TexCoord));
			fout.write(reinterpret_cast<const char*>(&v.Normal), sizeof(v.Normal));
			fout.write(reinterpret_cast<const char*>(&v.Tangent), sizeof(v.Tangent));
			fout.write(reinterpret_cast<const char*>(&v.BiNormal), sizeof(v.BiNormal));
		}

		// 🔹 인덱스 버퍼 저장
		fout.write(reinterpret_cast<const char*>(m_Inidices.data()), sizeof(uint32_t) * indexCount);
		fout.close();

		DV_LOG(StaticMesh, info, "[::SaveToFile] 메시 저장 완료: {}", filepath.string());
		return true;
	}
}
