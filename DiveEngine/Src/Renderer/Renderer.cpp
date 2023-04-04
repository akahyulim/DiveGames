#include "DivePch.h"
#include "Renderer.h"
#include "View.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/ShaderVariation.h"
#include "Graphics/InputLayout.h"
#include "IO/Log.h"

namespace Dive
{
	static std::vector<View*> s_Views;

	static ShaderVariation* s_pBasicVertexShader = nullptr;
	static ShaderVariation* s_pBasicPixelShader = nullptr;

	bool Renderer::Initialize()
	{
		if (!createShaders())
		{
			DV_CORE_WARN("셰이더 객체 생성에 실패하였습니다.");
		}
		
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

	bool Renderer::createShaders()
	{
		s_pBasicVertexShader = new ShaderVariation;
		if (!s_pBasicVertexShader->CompileAndCreate(eShaderType::VertexShader, "Assets/Shaders/basic.hlsl", eVertexType::Model))
			return false;

		s_pBasicPixelShader = new ShaderVariation;
		if (!s_pBasicPixelShader->CompileAndCreate(eShaderType::PixelShader, "Assets/Shaders/basic.hlsl"))
			return false;

		DV_CORE_INFO("Created Shaders");

		return true;
	}
}