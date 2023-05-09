#pragma once
#include "Graphics.h"

namespace Dive
{
	class InputLayout
	{
	public:
		InputLayout();
		~InputLayout();

		bool Create(ID3D10Blob* pShaderBuffer, eVertexType type);

		ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }

	private:

	private:
		eVertexType m_VertexType;

		ID3D11InputLayout* m_pInputLayout;
	};
}