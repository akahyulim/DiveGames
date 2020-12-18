#pragma once
#include "DivePch.h"
#include "Core/Object.h"

namespace Dive
{
	class Context;

	enum class eResourceType
	{
		UnKnown,
		Texture,
		Texture2D,
		TextureCube,
		Model,
	};

	class Resource : public Object
	{
		DIVE_OBJECT(Resource, Object);

	public:
		Resource(Context* context, eResourceType type) : Object(context), m_type(type) {}
		virtual ~Resource() = default;

		virtual bool SaveToFile(const std::string& path)	{ return true; }
		virtual bool LoadFromFile(const std::string& path)	{ return true; }

		const std::string& GetName()	const { return m_name; }
		eResourceType GetType()			const { return m_type; }

	protected:
		std::string m_name;
		std::string m_path;
		eResourceType m_type	= eResourceType::UnKnown;
	};
}