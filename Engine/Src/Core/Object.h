#pragma once
#include "../Helper/Log.h"
#include <string>
#include <atomic>
#include <typeinfo>


namespace Dive
{
	static const uint32_t INVALID_ID = 0;

	class Object
	{
	public:
		Object()
		{
			static std::atomic<uint32_t> next(INVALID_ID + 1);
			m_instanceID = next.fetch_add(1);
		}
		virtual ~Object() {}

		uint32_t GetInstanceID() const { return m_instanceID; }
		void SetInstanceID(uint32_t id) 
		{ 
			if (INVALID_ID == id)
			{
				CORE_ERROR("");
				return;
			}

			m_instanceID = id;
		}

		std::string GetName() const { return m_name; }
		void SetName(const std::string& name) { m_name = name; }

	private:
		Object(const Object&)				= delete;
		Object& operator=(const Object&)	= delete;

	private:
		uint32_t m_instanceID	= INVALID_ID;
		std::string m_name		= "";
	};
}