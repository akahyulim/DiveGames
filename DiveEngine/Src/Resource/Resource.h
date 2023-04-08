#pragma once
#include "Core/Object.h"

namespace Dive
{
	enum class eResourceType
	{
		Image,
		Texture2D,
		RenderTexture,
		TextureCube,
		Material,
		Model,
		Shader,
		Unknown
	};

	class Resource : public Object
	{
	public:
		Resource(eResourceType type = eResourceType::Unknown) : m_ResourceType(type) {}
		~Resource() override = default;

		virtual bool LoadFromFile(const std::string& filePath) { return true; }
		virtual bool SaveToFile(const std::string& filePath) { return true; }

		eResourceType GetResourceType() const { return m_ResourceType; }

		template<typename T>
		static constexpr eResourceType TypeToEnum();

	protected:
		eResourceType m_ResourceType;
	};
}