#include "stdafx.h"
#include "Sandbox.h"

using namespace Dive;

namespace Sandbox
{
	void Sandbox::Initialize()
	{
		Runtime::Initialize();

		// �߰� ó��???

		SetActivePath(&m_renderPath);
	}
}