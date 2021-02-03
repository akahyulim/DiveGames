#pragma once
#include "DivePch.h"
#include "Resource/Resource.h"
#include "GraphicsEnums.h"


namespace Dive
{
	class Context;
	class ConstantBuffer;

	class Material : public Resource
	{
		DIVE_OBJECT(Material, Resource);

	public:
		Material(Context* context)
			: Resource(context),
			m_shaderPass(eShaderPassType::Undefined),
			m_constantBuffer(nullptr),
			m_bDirty(true)
		{
			m_resourceType = eResourceType::Material;
		}
		~Material() = default;

		eShaderPassType GetShaderPass() const		{ return m_shaderPass; }
		void SetShaderPass(eShaderPassType type)	{ m_shaderPass = type; }

	protected:
		eShaderPassType m_shaderPass;
		ConstantBuffer* m_constantBuffer;

		bool m_bDirty;
	};
}