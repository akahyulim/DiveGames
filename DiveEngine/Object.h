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
			: mTypeHash(typeHash)
		{
			static std::atomic<unsigned int> next{ INVALID_ID + 1 };
			mID = next.fetch_add(1);
		}
		virtual ~Object() {}

		unsigned int GetID() const { return mID; }
		void SetID(unsigned int id) 
		{ 
			if (INVALID_ID == id)
			{
				CORE_ERROR("");
				return;
			}

			mID = id;
		}

		size_t GetTypeHash() const { return mTypeHash; }

	private:
		Object(const Object&)				= delete;
		Object& operator=(const Object&)	= delete;

	private:
		unsigned int mID = INVALID_ID;
		size_t mTypeHash = INVALID_ID;
	};
}