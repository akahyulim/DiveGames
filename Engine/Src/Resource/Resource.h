#pragma once
#include "DivePch.h"
#include "Core/Object.h"

namespace Dive
{
	class Context;

	// RenderTargetTexture, DepthStencilTexture, CubeTexture·Î º¯°æ?
	enum class eResourceType
	{
		UnKnown,
		Texture,
		Texture2D,
		TextureCube,
		Material,
		Mesh,
	};

	class Resource : public Object
	{
		DIVE_OBJECT(Resource, Object);

	public:
		Resource(Context* context)
			: Object(context),
			m_resourceType(eResourceType::UnKnown)
		{}
		virtual ~Resource() = default;

		virtual bool SaveToFile(const std::string& path)	{ return true; }
		virtual bool LoadFromFile(const std::string& path)	{ return true; }

		const std::string& GetResourceName()	const { return m_resourceName; }
		const std::string& GetResourcePath()	const { return m_resourcePath; }
		eResourceType GetResourceType()			const { return m_resourceType; }

		void SetResourceName(const std::string& name) { m_resourceName = name; }

	protected:
		std::string m_resourceName;
		std::string m_resourcePath;
		eResourceType m_resourceType;
	};
}