#include "gtest/gtest.h"
#include "Core/DiveEngine.h"

// events
class AddEvent : public Dive::Event
{
public:
	AddEvent(int a, int b) : m_a(a), m_b(b) {}
	int m_a;
	int m_b;
};

class MulEvent : public Dive::Event
{
public:
	MulEvent(int a, int b) : m_a(a), m_b(b) {}
	int m_a;
	int m_b;
};

// object
class Foo
{
public:
	Foo()
	{
		auto& dispatcher = Dive::EventDispatcher::GetInstace();
		dispatcher.Subscribe(this, &Foo::OnAdd);
		dispatcher.Subscribe(this, &Foo::OnMul);
	}

	void OnAdd(AddEvent* evnt)
	{
		m_add = evnt->m_a + evnt->m_b;
	}

	void OnMul(MulEvent* evnt)
	{
		m_mul = evnt->m_a * evnt->m_b;
	}

	int m_add;
	int m_mul;
};

class Boo
{
public:
	Boo()
	{
		auto& dispatcher = Dive::EventDispatcher::GetInstace();
		dispatcher.Subscribe(this, &Boo::OnMul);
	}

	void OnMul(MulEvent* evnt)
	{
		m_result = evnt->m_a * evnt->m_b;
	}

	int m_result;
};

// tests
TEST(Events, CallMethod)
{
	Foo foo;
	Boo boo;
	
	auto& dispatcher = Dive::EventDispatcher::GetInstace();
	AddEvent add(22, 33);
	MulEvent mul(5, 4);
	dispatcher.Fire(&add);
	dispatcher.Fire(&mul);

	ASSERT_EQ(55, foo.m_add);
	ASSERT_EQ(20, foo.m_mul);
	ASSERT_EQ(20, boo.m_result);
}