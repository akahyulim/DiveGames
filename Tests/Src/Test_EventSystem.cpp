#include "gtest/gtest.h"
#include "Core/DiveEngine.h"

// events
class AddEvent : public Dive::IEvent
{
public:
	AddEvent(int a, int b) : m_A(a), m_B(b) {}
	int m_A;
	int m_B;
};

class MulEvent : public Dive::IEvent
{
public:
	MulEvent(int a, int b) : m_A(a), m_B(b) {}
	int m_A;
	int m_B;
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
		m_Add = evnt->m_A + evnt->m_B;
	}

	void OnMul(const MulEvent* evnt)
	{
		m_Mul = evnt->m_A * evnt->m_B;
	}

	int m_Add = 0;
	int m_Mul = 0;
};

class Boo
{
public:
	void OnAdd(const AddEvent* evnt)
	{
		m_Add = evnt->m_A + evnt->m_B;
	}

	void OnMul(const MulEvent* evnt)
	{
		m_Mul = evnt->m_A * evnt->m_B;
	}

	int m_Add = 0;
	int m_Mul = 0;
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

	ASSERT_EQ(55, foo.m_Add);
	ASSERT_EQ(20, foo.m_Mul);
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

	ASSERT_EQ(33, boo.m_Add);
	ASSERT_EQ(28, boo.m_Mul);
}