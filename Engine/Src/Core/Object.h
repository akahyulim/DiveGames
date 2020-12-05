#pragma once

namespace Dive
{
	static unsigned int g_InstanceID = 0;

	class Object
	{
	public:
		Object()
		{
			m_id = GenerateID();
		}
		virtual ~Object() {}

		unsigned int GetID() const { return m_id; }

		static unsigned int GenerateID() { return ++g_InstanceID; }

		// 실제 적용을 위해선 pure함수여야 한다.
		// 일단  subsystem에 적용
		//virtual size_t GetType() const {}
		//virtual std::string GetTyepName() const {}

	protected:
		// Orho3D에선 개별 object에 id가 존재하지 않는다.
		// 대신 Scene이 Component들에게 특정 범위의 ID를 할당해주는듯 하다.
		unsigned int m_id = 0;
	};
}
