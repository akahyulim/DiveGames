#include "DivePch.h"
#include "Mesh.h"
#include "Core/FileSystemHelper.h"
#include "Core/FileStream.h"

namespace Dive
{
	Mesh::Mesh(Context * context)
		: Resource(context),
		m_vertexBuffer(nullptr),
		m_indexBuffer(nullptr)
	{
		m_resourceType = eResourceType::Mesh;
	}

	Mesh::~Mesh()
	{
		SAFE_DELETE(m_indexBuffer);
		SAFE_DELETE(m_vertexBuffer);
	}

	bool Mesh::SaveToFile(const std::string & filepath)
	{
		// 이름과 경로 저장?

		auto file = std::make_unique<FileStream>(filepath, eFileStreamMode::Write);
		if (file->IsOpen())
			return false;

		// normal?
		// vertices
		// indices

		file->Close();

		return true;
	}
	
	bool Mesh::LoadFromFile(const std::string & filepath)
	{
		if (filepath.empty() || !File::IsExistFile(filepath) || !File::IsEngineMeshFile(filepath))
		{
			CORE_ERROR("존재하지 않거나 지원하지 않는 형식의 파일을 전달받았습니다. - {:s}", filepath);
			return false;
		}



		return true;
	}
}