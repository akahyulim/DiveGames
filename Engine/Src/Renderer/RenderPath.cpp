#include "divepch.h"
#include "RenderPath.h"
#include "Core/Context.h"
#include "IO/Log.h"

// �ᱹ ũ�Ժ��� ����Ʈ�� ���۵� �� ���� �����н��� ����� ���� �Ǵµ�
// 1. ���� ��� �����Ͽ� ������ ���ΰ�?
// 2. ������ ������ ���(xml vs yaml) ����ȭ�� ���ΰ�?
// ���� �� ������ �ʿ��ϴ�.
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