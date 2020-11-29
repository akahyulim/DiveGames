#pragma once
#include "DivePch.h"
#include "Core/Object.h"

namespace Dive
{
	enum class eResourceType
	{
		UnKnown,
		Texture,
		Texture2D,
		TextureCube,
	};

	enum class eLoadState
	{
		Idle,
	};

	class Resource : public Object
	{
	public:
		Resource(eResourceType type) : m_type(type) {}
		virtual ~Resource() = default;

		virtual bool SaveToFile(const std::string& path)	{ return true; }
		virtual bool LoadFromFile(const std::string& path)	{ return true; }

		const std::string& GetName()	const { return m_name; }
		eResourceType GetType()			const { return m_type; }
		eLoadState GetLoadState()		const { return m_loadState; }

	protected:
		std::string m_name;
		std::string m_Path;
		eResourceType m_type	= eResourceType::UnKnown;
		eLoadState m_loadState	= eLoadState::Idle;
	};
}