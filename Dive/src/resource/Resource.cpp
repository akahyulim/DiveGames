#include "stdafx.h"
#include "Resource.h"
#include "core/FileUtils.h"

namespace Dive
{	void Resource::SetFilepath(const std::string& filepath)
	{
		m_Name = FileUtils::GetFileName(filepath);
		// ���� ����� ���� ���� �ʿ�
		// �̸� ���ؼ� FileUtils�� ���� �Լ��� ������ �ϴµ�
		// ������� ������ �Ǵ� WorkingDir ������ �ʿ��ϴ�.
		m_Filepath = filepath;
	}
}