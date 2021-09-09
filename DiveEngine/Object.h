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
		Object()
		{
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

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
		Object(const Object&)				= delete;
		Object& operator=(const Object&)	= delete;

	private:
		unsigned int m_InstanceID	= INVALID_ID;
		std::string m_Name			= "";
	};
}