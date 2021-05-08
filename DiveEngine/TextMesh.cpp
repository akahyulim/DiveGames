#include "TextMesh.h"

namespace Dive
{
	TextMesh::TextMesh()
	{
		// ��� ���� �ʱ�ȭ

		// Resource Manager �����κ��� Default Font�� ���´�.
		// Font��ü�� Renderer ��� Face���� �����ȴ�.
		// �׸��� �ش� Font�κ��� Size ������ �����Ѵ�.
	}

	TextMesh::~TextMesh()
	{
	}
	
	bool TextMesh::SetFont(const Dive_Font* pFont)
	{
		assert(pFont != nullptr);

		if (m_pFont != pFont)
			m_pFont = const_cast<Dive_Font*>(pFont);

		return true;
	}

	// �ᱹ ascii�� unicode�� ������� �Ѵ�.
	bool TextMesh::SetText(const std::string& text)
	{
		// �̰� �� �����ϴ�.
		// �ϴ� ���� ��ü�� �����ؾ� �Ѵ�. �׷��� ���� �Է��� �ٽ� �׸��� ���� �� �ִ�.
		// �׷��� ���� Font�� Face�� Size�� ����Ǹ�
		// �̵��� �ٽ� �ؽ��Ŀ� ���ܾ� �Ѵ�.
		// �׸��� �̿� ���� ���۵� �ٽ� ��������� �Ѵ�.
		// ���� �� ������ ���۸� �����ϴ°� �´��� �����غ� �ʿ䰡 �ִ�.

		return true;
	}

	void TextMesh::SetFontSize(unsigned int size)
	{
		if (m_fontSize == size)
			return;

		// Font�� �ٽ� �����ؾ� �Ѵ�.

		m_fontSize = size;
	}
}