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
			: mTypeHash(typeHash)
		{
			static std::atomic<unsigned int> next{ INVALID_ID + 1 };
			mInstanceID = next.fetch_add(1);
		}
		virtual ~Object() {}

		unsigned int GetInstanceID() const { return mInstanceID; }
		void SetInstanceID(unsigned int id) 
		{ 
			if (INVALID_ID == id)
			{
				CORE_ERROR("");
				return;
			}

			mInstanceID = id;
		}

		size_t GetTypeHash() const { return mTypeHash; }

		std::string GetName() const { return mName; }
		void SetName(const std::string& name) { mName = name; }

	private:
		Object(const Object&)				= delete;
		Object& operator=(const Object&)	= delete;

	private:
		unsigned int mInstanceID	= INVALID_ID;
		size_t mTypeHash			= INVALID_ID;
		std::string mName			= "";
	};
}