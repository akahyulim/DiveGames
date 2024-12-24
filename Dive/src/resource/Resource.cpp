#include "stdafx.h"
#include "Resource.h"
#include "core/FileUtils.h"

namespace Dive
{	
	std::unordered_map<size_t, std::unordered_map<std::string, std::shared_ptr<DvResource>>> DvResource::s_Resources;
	std::mutex DvResource::s_Mutex;

	//===========================================================================================

	void Resource::SetFilepath(const std::string& filepath)
	{
		m_Name = FileUtils::GetFileName(filepath);
		// ���� ����� ���� ���� �ʿ�
		// �̸� ���ؼ� FileUtils�� ���� �Լ��� ������ �ϴµ�
		// ������� ������ �Ǵ� WorkingDir ������ �ʿ��ϴ�.
		m_Filepath = filepath;
	}
}