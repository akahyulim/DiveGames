#pragma once

namespace Dive
{
	class Object
	{
	public:
		Object();
		virtual ~Object() = default;

		uint64_t GetInstanceID() const { return m_InstanceID; }
		void SetInstanceID(uint64_t id) { m_InstanceID = id; }
		static uint64_t GenerateInstanceID();

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		bool operator!=(const Object& rhs);
		bool operator==(const Object& rhs);

		// 유니티의 interface지만
		// virtual과 static은 양립할 수 없다...
		// 직접 derived type을 확인한 후 복사하는 함수를 만들 순 있다.
		// 일단은 GameObject에 생성하자.
		//static Object* Instantiate(Object* pOriginal);
		//static void Destroy(Object* pObj);

	private:
		Object(const Object&) = delete;

	protected:
		unsigned long long m_InstanceID;
		std::string m_Name;
	};
}