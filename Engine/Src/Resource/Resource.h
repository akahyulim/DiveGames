#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Resource : public Object
	{
		DV_CLASS(Resource, Object);

	public:
		Resource();
		virtual ~Resource() = default;

		virtual bool LoadFromFile(const std::string& fileName) { return true; }
		virtual bool SaveToFile(const std::string& fileName) { return true; }

		void SetName(const std::string& name);
		const std::string& GetName() const { return m_Name; }
		size_t GetNameHash() const { return m_NameHash; }

	private:
		std::string m_Name;
		size_t m_NameHash;
	};
}