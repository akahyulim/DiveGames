#pragma once

namespace Dive
{
	static unsigned int g_InstanceID = 0;

	class Object
	{
	public:
		Object()
		{
			m_ID = GenerateID();
		}
		virtual ~Object() {}

		unsigned int GetID() const { return m_ID; }

		static unsigned int GenerateID() { return ++g_InstanceID; }

	protected:
		unsigned int m_ID = 0;
	};
}