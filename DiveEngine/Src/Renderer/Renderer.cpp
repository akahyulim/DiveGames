#include "DivePch.h"
#include "Renderer.h"
#include "View.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "IO/Log.h"

namespace Dive
{
	static std::vector<View*> s_Views;

	bool Renderer::Initialize()
	{
		DV_CORE_TRACE("Renderer 초기화에 성공하였습니다.");

		return true;
	}

	void Renderer::Shutdown()
	{
		for (auto pView : s_Views)
			DV_DELETE(pView);


		DV_CORE_TRACE("Renderer 종료에 성공하였습니다.");
	}

	void Renderer::Update(float delta)
	{
		for (auto pView : s_Views)
			pView->Update(delta);
	}

	void Renderer::Render()
	{
		for (auto pView : s_Views)
			pView->Render();
	}

	void Renderer::SetView(uint32_t index, View* pView)
	{
		if (s_Views.size() >= index)
			s_Views.insert(s_Views.begin() + index, pView);
		else
			s_Views.emplace_back(pView);
	}
}