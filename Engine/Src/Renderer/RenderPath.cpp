#include "divepch.h"
#include "RenderPath.h"
#include "Core/Context.h"
#include "IO/Log.h"

// 결국 크게보면 포워트와 디퍼드 두 개의 렌더패스를 만들어 놓게 되는데
// 1. 둘을 어떻게 구분하여 구성할 것인가?
// 2. 구성한 사항을 어떻게(xml vs yaml) 파일화할 것인가?
// 연구 및 결정이 필요하다.
namespace Dive
{
	RenderPath::RenderPath(Context* pContext)
		: Resource(pContext)
	{
	}
	
	RenderPath::~RenderPath()
	{
	}

	bool RenderPath::LoadFromFile(const std::string& filepath)
	{
		return false;
	}
	
	bool RenderPath::SaveToFile(const std::string& filepath)
	{
		return false;
	}

	void RenderPath::AddCommand(const RenderPathCommand& command)
	{
		m_RenderPathCommands.emplace_back(command);
	}
}