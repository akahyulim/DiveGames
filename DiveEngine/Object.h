#pragma once
#include "Log.h"
#include <string>
#include <atomic>
#include <typeinfo>

namespace dive
{
	static const unsigned int INVALID_ID = 0;

	class Object
	{
	public:
		Object(size_t typeHash)
			: m_TypeHash(typeHash)
		{
			static std::atomic<unsigned int> next{ INVALID_ID + 1 };
			m_InstanceID = next.fetch_add(1);
		}
		virtual ~Object() {}

		unsigned int GetInstanceID() const { return m_InstanceID; }
		void SetInstanceID(unsigned int id) 
		{ 
			if (INVALID_ID == id)
			{
				CORE_ERROR("");
				return;
			}

			m_InstanceID = id;
		}

		size_t GetTypeHash() const { return m_TypeHash; }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
		Object(const Object&)				= delete;
		Object& operator=(const Object&)	= delete;

	private:
		unsigned int m_InstanceID	= INVALID_ID;
		size_t m_TypeHash			= INVALID_ID;
		std::string m_Name			= "";
	};
}