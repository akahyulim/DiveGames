#include "divepch.h"
#include "Runtime.h"
#include "DiveCore.h"
#include "Time.h"
#include "Log.h"

namespace Dive
{
	Runtime* Runtime::s_pInstance = nullptr;

	Runtime::Runtime()
	{
		DV_ASSERT(!s_pInstance);
		s_pInstance = this;

		Log::Initialize();
		Time::Initialize();
	}

	Runtime::~Runtime()
	{
	}

	void Runtime::Initialize()
	{
	}

	void Runtime::Tick()
	{
	}
}