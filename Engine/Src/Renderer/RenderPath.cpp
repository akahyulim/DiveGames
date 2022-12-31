#include "divepch.h"
#include "RenderPath.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

// 결국 크게보면 포워드와 디퍼드 두 개의 렌더패스를 만들어 놓게 되는데
// 1. 둘을 어떻게 구분하여 구성할 것인가?
// 2. 구성한 사항을 어떻게(xml vs yaml) 파일화할 것인가?
// 연구 및 결정이 필요하다.
namespace Dive
{
	RenderPath::RenderPath(Context* pContext)
		: Resource(pContext)
	{
		DV_LOG_ENGINE_TRACE("RenderPath 생성");
	}
	
	RenderPath::~RenderPath()
	{
		DV_LOG_ENGINE_TRACE("RenderPath 소멸 완료");
	}

	bool RenderPath::LoadFromFile(const std::string& filepath)
	{
		// RenderTarget과 RenderPathCommand를 Clear한 후
		// 각각의 Load함수를 호출하여 구성한다.

		// 아래의 내용은 command
		// 굳이 구조체를 클래스로 바꿔 Load함수를 가진게 하느니
		// 그냥 여기에서 직접 하는 것도 괜찮은 듯 하다.

		// type
		// tag
		// enabled
		// metadata

		// case type
		// clear - color, depth, stencil
		// scenepass - pass, sort, marktostencil, vertexlights

		// output
		// 일단 크기를 1로 리사이즈한 후 viewport, FACE_POSITIVE_X로 설정
		// output이 있다면 output_[0].first에 output 요소를 저장
		// face가 있다면 output_[0].second에 face 요소를 저장
		// depthstencil이 있다면 depthstencilName_에 depthstencil 요소를 저장

		return false;
	}
	
	bool RenderPath::SaveToFile(const std::string& filepath)
	{
		return false;
	}

	RenderPathCommand* RenderPath::GetCommand(uint32_t index)
	{
		DV_ASSERT(index >= 0);

		return index < GetCommandCount() ? &m_Commands[index] : nullptr;
	}

	void RenderPath::AddCommand(const RenderPathCommand& command)
	{
		m_Commands.emplace_back(command);
	}
}