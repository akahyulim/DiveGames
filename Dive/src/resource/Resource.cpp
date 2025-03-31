#include "stdafx.h"
#include "Resource.h"
#include "core/FileUtils.h"

namespace Dive
{	
	void Resource::SetFilepath(const std::filesystem::path& filepath)
	{
		m_Name = filepath.stem().string();
		// ���� ����� ���� ���� �ʿ�
		// �̸� ���ؼ� FileUtils�� ���� �Լ��� ������ �ϴµ�
		// ������� ������ �Ǵ� WorkingDir ������ �ʿ��ϴ�.
		m_Filepath = filepath;
	}
}