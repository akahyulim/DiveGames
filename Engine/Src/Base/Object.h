#pragma once

namespace Dive
{
	// urho3d의 경우 object는 stringHash, event 처리 및 GetSubSystem이 주 역할이며,
	// 대부분의 class의 base이다.
	// 반면, id의 경우 node(=game object), component만 가지며
	// scene에서 각각 관리한다.
	// 리소스의 경우 이름을 통해 stringHash를 사용하는 듯 하다.
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