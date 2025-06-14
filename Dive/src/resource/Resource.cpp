#include "stdafx.h"
#include "Resource.h"
#include "core/FileUtils.h"
#include "core/InstanceID.h"

namespace Dive
{
	Resource::Resource(const std::string& name)
		: m_InstanceID(InstanceID())
		, m_Name(name)
	{
		DV_LOG(Resource, trace, "Create Resource - name: {0}, instanceID: {1}", m_Name, m_InstanceID);
	}

	Resource::Resource(UINT64 instanceID, const std::string& name)
		: m_InstanceID(instanceID)
		, m_Name(name)
	{ 
		DV_LOG(Resource, trace, "Create Resource - name: {0}, instanceID: {1}", m_Name, m_InstanceID);
	}

	Resource::~Resource()
	{
		DV_LOG(Resource, trace, "Delete Resource - name: {0}, instanceID: {1}", m_Name, m_InstanceID);
	}

	void Resource::SetFilepath(const std::filesystem::path& filepath)
	{
		m_Name = filepath.stem().string();
		// 추후 상대경로 저장 수정 필요
		// 이를 위해서 FileUtils에 관련 함수를 만들어야 하는데
		// 상대경로의 기준으 되는 WorkingDir 설정도 필요하다.
		m_Filepath = filepath;
	}
}
