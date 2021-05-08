#pragma once
#include "Renderer.h"
#include "Dive_Font.h"
#include <string>

namespace Dive
{
	// ���� Mesh�� ������???
	class TextMesh
	{
	public:
		TextMesh();
		~TextMesh();

		bool SetFont(const Dive_Font* pFont);

		bool SetText(const std::string& text);

		// ���� ũ�� ���� �Լ��� ȣ���Ѵٸ�
		// Font���� �ٽ� �ؽ��ĸ� ������ �Ѵ�.
		// �׸��� �� ��� ���� ��±����� ����� ũ�⸦ ����޾ƾ� �Ѵ�.
		// �Ϲ������� �̷��� ���� �翬�ϴ�.
		void SetFontSize(unsigned int size);

		ID3D11Buffer* GetVertexBuffer();
		ID3D11Buffer* GetIndexBuffer();
		// �����Լ� ȣ���� �� �ۿ� ����.
		ID3D11ShaderResourceView* GetAtlas();

	private:

	private:
		ID3D11Buffer* m_pVertexBuffer = nullptr;
		ID3D11Buffer* m_pIdexBuffer = nullptr;

		// Font�� �ᱹ Resource�� �Ǿ�� �Ѵ�.
		Dive_Font* m_pFont = nullptr;

		// state�� ���� ������ �ϳ�? ������ �� ������ �ϳ�?

		unsigned int m_fontSize;
	};
}