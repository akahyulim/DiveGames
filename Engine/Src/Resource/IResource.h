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

	class IResource : public Object
	{
	public:
		IResource(eResourceType type) : m_Type(type) {}
		virtual ~IResource() = default;

		virtual bool SaveToFile(const std::string& path)	{ return true; }
		virtual bool LoadFromFile(const std::string& path)	{ return true; }

		const std::string& GetName()	const { return m_Name; }
		eResourceType GetType()			const { return m_Type; }
		eLoadState GetLoadState()		const { return m_LoadState; }

	protected:
		std::string m_Name;
		std::string m_Path;
		eResourceType m_Type	= eResourceType::UnKnown;
		eLoadState m_LoadState	= eLoadState::Idle;
	};
}