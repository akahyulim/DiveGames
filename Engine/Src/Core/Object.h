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

	protected:
		unsigned int m_id = 0;
	};
}