#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class VertexBuffer;
	class Shader;

	class InputLayout : public Object
	{
		DIVE_OBJECT(InputLayout, Object)

	public:
		explicit InputLayout(Context* pContext, VertexBuffer** ppBuffers, Shader* pVertexShader);
		~InputLayout() override;

		ID3D11InputLayout* GetInputLayout() const { return m_pInputLayout; }

	private:
		ID3D11InputLayout* m_pInputLayout;
	};
}