#include "stdafx.h"
#include "Resource.h"
#include "core/FileUtils.h"

namespace Dive
{	
	std::unordered_map<size_t, std::unordered_map<std::string, std::shared_ptr<DvResource>>> DvResource::s_Resources;
	std::mutex DvResource::s_Mutex;

	//===========================================================================================

	void Resource::SetFilepath(const std::filesystem::path& filepath)
	{
		m_Name = filepath.stem().string();
		// ���� ����� ���� ���� �ʿ�
		// �̸� ���ؼ� FileUtils�� ���� �Լ��� ������ �ϴµ�
		// ������� ������ �Ǵ� WorkingDir ������ �ʿ��ϴ�.
		m_Filepath = filepath;
	}
}