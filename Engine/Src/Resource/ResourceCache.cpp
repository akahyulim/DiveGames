#include "divepch.h"
#include "ResourceCache.h"
#include "Core/CoreDefs.h"
#include "Core/Context.h"
#include "IO/FileStream.h"
#include "IO/FileSystem.h"

namespace Dive 
{
	ResourceCache::ResourceCache(Context* pContext)
		: Object(pContext)
	{
	}

	ResourceCache::~ResourceCache()
	{
		RemoveAllResources();
	}

	// 매뉴얼리소스는 반드시 이름이 존재해야 한다.
	bool ResourceCache::AddManualResource(Resource* pResource)
	{
		if (!pResource)
		{
			DV_LOG_ENGINE_ERROR("잘못된 인자를 전달받아 리소스를 추가할 수 없습니다.");
			return false;
		}

		if (pResource->GetName().empty())
		{
			DV_LOG_ENGINE_ERROR("전달받은 리소스에 이름이 존재하지 않아 추가할 수 없습니다.");
			return false;
		}

		m_ResourceGroups[pResource->GetType().Value()][pResource->GetNameHash().Value()] = pResource;

		return true;
	}

	// 일단은 전체, 상대 경로로 받은 후 로드하도록 구현을 수정했다.
	// 그리고 현재 fixResourceXXX() 함수들의 사용을 제거했으며,
	// 리소스 폴더 설정도 무시한다.
	// 실제로는 외부 경로 파일일 경우 리소스 폴더로 복사한 후 처리해야 한다.
	Resource* ResourceCache::GetResource(StringHash type, const std::string& name)
	{
		auto fileName = FileSystem::GetFileNameAndExtension(name);
		StringHash nameHash(fileName);

		auto* pExistedResource = findResource(type, nameHash);
		if (pExistedResource)
			return pExistedResource;

		auto* pNewResource = static_cast<Resource*>(m_pContext->CreateObject(type));
		if (!pNewResource->LoadFromFile(name))
		{
			DV_DELETE(pNewResource);
			return nullptr;
		}

		pNewResource->SetName(fileName);
		
		m_ResourceGroups[type.Value()][nameHash.Value()] = pNewResource;

		return pNewResource;
	}

	void ResourceCache::GetResources(StringHash type, std::vector<Resource*>& outResources)
	{
		outResources.clear();

		auto i = m_ResourceGroups.find(type.Value());
		if (i != m_ResourceGroups.end())
		{
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				outResources.emplace_back(j->second);
			}
		}
	}

	void ResourceCache::RemoveResource(StringHash type, const std::string& name)
	{
		StringHash nameHash(name);
		auto* pExistedReosurce = findResource(type, nameHash);
		if (!pExistedReosurce)
			return;

		DV_DELETE(pExistedReosurce);
		m_ResourceGroups[type.Value()].erase(nameHash.Value());
	}

	void ResourceCache::RemoveResources(StringHash type)
	{
		auto i = m_ResourceGroups.find(type.Value());
		if (i == m_ResourceGroups.end())
			return;

		for (auto j = i->second.begin(); j != i->second.end(); ++j)
			DV_DELETE(j->second);

		m_ResourceGroups.erase(i);
	}

	void ResourceCache::RemoveAllResources()
	{
		for (auto i = m_ResourceGroups.begin(); i != m_ResourceGroups.end(); ++i)
		{
			for (auto j = i->second.begin(); j != i->second.end(); ++j)
			{
				DV_DELETE(j->second);
			}
		}

		m_ResourceGroups.clear();
	}

	bool ResourceCache::AddResourceDir(const std::string& pathName)
	{
		if (pathName.empty())
			return false;

		if (!FileSystem::DirExists(pathName))
		{
			DV_LOG_ENGINE_ERROR("전달받은 경로({:s})가 존재하지 않아 리소스 폴더로 저장할 수 없습니다.", pathName);
			return false;
		}

		auto fixedPath = fixResourceDirName(pathName);

		auto it = std::find(m_ResourceDirs.begin(), m_ResourceDirs.end(), fixedPath);
		if (m_ResourceDirs.empty() || it == m_ResourceDirs.end())
			m_ResourceDirs.emplace_back(fixedPath);

		return true;
	}

	void ResourceCache::RemoveResourceDir(const std::string& pathName)
	{
		if (pathName.empty())
			return;

		auto fixedPath = fixResourceDirName(pathName);

		auto it = m_ResourceDirs.begin();
		for (it; it != m_ResourceDirs.end();)
		{
			if ((*it) == fixedPath)
				it = m_ResourceDirs.erase(it);
			else
				++it;
		}
	}

	Resource* ResourceCache::findResource(StringHash type, StringHash nameHash)
	{
		auto i = m_ResourceGroups.find(type.Value());
		if (i == m_ResourceGroups.end())
			return nullptr;

		auto j = i->second.find(nameHash.Value());
		if (j == i->second.end())
			return nullptr;

		return j->second;
	}

	// 절대 경로 혹은 현재 위치에서의 상대 경로를 받아 
	// '리소스 폴더/이름.확장자'형태로 리턴한다.
	std::string ResourceCache::fixResourceName(const std::string& name) const
	{
		auto pathName = FileSystem::GetPath(name);

		if (FileSystem::IsAbsolutePath(name))
		{
			for (unsigned int i = 0; i < (unsigned int)m_ResourceDirs.size(); ++i)
			{
				if (m_ResourceDirs[i] == pathName)
					return FileSystem::AddTrailingSlash(FileSystem::GetLastPath(name)) + FileSystem::GetFileNameAndExtension(name);
			}
		}
		else
		{
			auto fullPath = FileSystem::GetCurrentDir() + pathName;
			
			for (unsigned int i = 0; i < (unsigned int)m_ResourceDirs.size(); ++i)
			{
				if (m_ResourceDirs[i] == fullPath)
					return FileSystem::AddTrailingSlash(FileSystem::GetLastPath(name)) + FileSystem::GetFileNameAndExtension(name);
			}
		}

		return std::string();
	}

	std::string ResourceCache::fixResourceDirName(const std::string& name) const
	{
		auto fixedPath = FileSystem::AddTrailingSlash(name);

		if (!FileSystem::IsAbsolutePath(name))
			fixedPath = FileSystem::GetCurrentDir() + fixedPath;

		return fixedPath;
	}

	FileStream* ResourceCache::getFileStream(const std::string& name) const
	{
		FileStream* pFileStream = nullptr;
		auto filepath = FileSystem::GetFileNameAndExtension(name);

		for (unsigned int i = 0; i < (unsigned int)m_ResourceDirs.size(); ++i)
		{
			filepath = m_ResourceDirs[i] + filepath;
			if (FileSystem::FileExists(filepath))
			{
				pFileStream = new FileStream;
				if (!pFileStream->Open(filepath, eFileStreamMode::Read))
					DV_DELETE(pFileStream);
			}
		}

		return pFileStream;
	}
}