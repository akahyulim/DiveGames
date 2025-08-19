#include "stdafx.h"
#include "Resource.h"
#include "core/FileUtils.h"

namespace Dive
{
	Resource::Resource(const std::string& name)
		: Object(name)
	{
		DV_LOG(Resource, info, "생성: {}, {}", GetName(), GetInstanceID());
	}

	Resource::~Resource()
	{
		DV_LOG(Resource, info, "소멸: {}. {}", GetName(), GetInstanceID());
	}

	std::string Resource::GetName() const
	{
		if (m_filepath.empty())
			return Object::GetName();

		return m_filepath.stem().string();
	}

	void Resource::SetName(const std::string& name)
	{
		if (m_filepath.empty())
		{
			Object::SetName(name);
			return;
		}

		auto extension = m_filepath.extension().string();
		std::string newName = name + extension;
		m_filepath.replace_filename(newName);
	}
}
