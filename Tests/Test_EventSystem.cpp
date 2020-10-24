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
		auto& dispatcher = Dive::EventSystem::GetInstance();
		dispatcher.Subscribe(this, &Foo::OnAdd);
		dispatcher.Subscribe(this, &Foo::OnMul);
	}

	void OnAdd(const AddEvent* evnt)
	{
		m_add = evnt->m_a + evnt->m_b;
	}

	void OnMul(const MulEvent* evnt)
	{
		m_mul = evnt->m_a * evnt->m_b;
	}

	int m_add = 0;
	int m_mul = 0;
};

class Boo
{
public:
	void OnAdd(const AddEvent* evnt)
	{
		m_add = evnt->m_a + evnt->m_b;
	}

	void OnMul(const MulEvent* evnt)
	{
		m_mul = evnt->m_a * evnt->m_b;
	}

	int m_add = 0;
	int m_mul = 0;
};

// tests
TEST(EventSystem, CallMethod)
{
	Foo foo;
	
	auto& bus = Dive::EventSystem::GetInstance();
	AddEvent add(22, 33);
	MulEvent mul(5, 4);
	bus.Fire(&add);
	bus.Fire(&mul);

	ASSERT_EQ(55, foo.m_add);
	ASSERT_EQ(20, foo.m_mul);
}

TEST(EventSystem, Subscribe)
{
	auto& bus = Dive::EventSystem::GetInstance();
	Boo boo;
	bus.Subscribe(&boo, &Boo::OnAdd);
	bus.Subscribe(&boo, &Boo::OnMul);

	AddEvent add(11, 22);
	MulEvent mul(7, 4);
	bus.Fire(&add);
	bus.Fire(&mul);

	ASSERT_EQ(33, boo.m_add);
	ASSERT_EQ(28, boo.m_mul);
}