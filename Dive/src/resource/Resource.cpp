#include "stdafx.h"
#include "Resource.h"
#include "core/FileUtils.h"

namespace Dive
{	
	void Resource::SetFilepath(const std::filesystem::path& filepath)
	{
		m_Name = filepath.stem().string();
		// 추후 상대경로 저장 수정 필요
		// 이를 위해서 FileUtils에 관련 함수를 만들어야 하는데
		// 상대경로의 기준으 되는 WorkingDir 설정도 필요하다.
		m_Filepath = filepath;
	}
}