#pragma once
#include <atomic>

namespace Dive
{
	static const unsigned int INVALID_ID = 0;

	class Object
	{
	public:
		Object()
		{
			static std::atomic<unsigned int> next{ INVALID_ID + 1 };
			m_id = next.fetch_add(1);
		}
		virtual ~Object() {}

		unsigned int GetID() const { return m_id; }
		// serialize 과정에 id를 저장할 거라면 SetID도 필요하다.

	private:
		Object(const Object&)				= delete;
		Object& operator=(const Object&)	= delete;

	private:
		unsigned int m_id = INVALID_ID;
	};
}