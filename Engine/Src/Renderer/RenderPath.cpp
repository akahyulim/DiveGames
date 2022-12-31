#include "divepch.h"
#include "RenderPath.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

// �ᱹ ũ�Ժ��� ������� ���۵� �� ���� �����н��� ����� ���� �Ǵµ�
// 1. ���� ��� �����Ͽ� ������ ���ΰ�?
// 2. ������ ������ ���(xml vs yaml) ����ȭ�� ���ΰ�?
// ���� �� ������ �ʿ��ϴ�.
namespace Dive
{
	RenderPath::RenderPath(Context* pContext)
		: Resource(pContext)
	{
		DV_LOG_ENGINE_TRACE("RenderPath ����");
	}
	
	RenderPath::~RenderPath()
	{
		DV_LOG_ENGINE_TRACE("RenderPath �Ҹ� �Ϸ�");
	}

	bool RenderPath::LoadFromFile(const std::string& filepath)
	{
		// RenderTarget�� RenderPathCommand�� Clear�� ��
		// ������ Load�Լ��� ȣ���Ͽ� �����Ѵ�.

		// �Ʒ��� ������ command
		// ���� ����ü�� Ŭ������ �ٲ� Load�Լ��� ������ �ϴ���
		// �׳� ���⿡�� ���� �ϴ� �͵� ������ �� �ϴ�.

		// type
		// tag
		// enabled
		// metadata

		// case type
		// clear - color, depth, stencil
		// scenepass - pass, sort, marktostencil, vertexlights

		// output
		// �ϴ� ũ�⸦ 1�� ���������� �� viewport, FACE_POSITIVE_X�� ����
		// output�� �ִٸ� output_[0].first�� output ��Ҹ� ����
		// face�� �ִٸ� output_[0].second�� face ��Ҹ� ����
		// depthstencil�� �ִٸ� depthstencilName_�� depthstencil ��Ҹ� ����

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