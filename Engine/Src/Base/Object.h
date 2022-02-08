#pragma once
#include "Base.h"
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
			m_ID = next.fetch_add(1);
		}
		virtual ~Object() {}

		uint32_t GetID() const { return m_ID; }
		void SetID(uint32_t id) 
		{ 
			if (INVALID_ID == id)
			{
				CORE_WARN("잘못된 ID로 설정을 시도하였습니다.");
				return;
			}

			m_ID = id;
		}

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
		Object(const Object&)				= delete;
		Object& operator=(const Object&)	= delete;

	private:
		uint32_t m_ID		= INVALID_ID;
		std::string m_Name	= "";
	};
}