#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;

	class DvModel : public Resource
	{
		RESOURCE_CLASS_TYPE(Model)

	public:

		bool LoadFromFile(const std::string& filepath) override;
		bool SaveToFile(const std::string& filepath) override;

	private:
	private:
		std::vector<VertexBuffer*> m_VertexBuffers;
		std::vector<IndexBuffer*> m_IndexBuffers;
		// meshs?
	};
}
