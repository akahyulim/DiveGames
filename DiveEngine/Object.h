#pragma once
#include "Log.h"
#include <atomic>
#include <typeinfo>

namespace dive
{
	static const unsigned int INVALID_ID = 0;

	class Object
	{
	public:
		Object(size_t typeHash)
			: m_typeHash(typeHash)
		{
			static std::atomic<unsigned int> next{ INVALID_ID + 1 };
			m_id = next.fetch_add(1);
		}
		virtual ~Object() {}

		unsigned int GetID() const { return m_id; }
		void SetID(unsigned int id) 
		{ 
			if (INVALID_ID == id)
			{
				CORE_ERROR("");
				return;
			}

			m_id = id;
		}

		size_t GetTypeHash() const { return m_typeHash; }

	private:
		Object(const Object&)				= delete;
		Object& operator=(const Object&)	= delete;

	private:
		unsigned int m_id = INVALID_ID;
		size_t m_typeHash = INVALID_ID;
	};
}