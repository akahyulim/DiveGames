#pragma once
#include "Core/Object.h"

namespace Dive
{
	enum class eResourceType
	{
		Image,
		RenderTexture,
		Texture2D,
		Material,
		Mesh,
		Shader,
		Unknown
	};

	class Resource : public Object
	{
	public:
		Resource(eResourceType type = eResourceType::Unknown) : m_ResourceType(type) {}
		virtual ~Resource() {}

		virtual bool LoadFromFile(const std::string& filePath) { return true; }
		virtual bool SaveToFile(const std::string& filePath) { return true; }

		eResourceType GetResourceType() const { return m_ResourceType; }

		template<typename T>
		static constexpr eResourceType TypeToEnum();

		std::string GetFilePath() const { return m_FilePath; }
		void SetFilePath(const std::string& filePath) { m_FilePath = filePath; }

	protected:
		eResourceType m_ResourceType;
		std::string m_FilePath;
	};
}